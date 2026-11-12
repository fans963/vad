#include "SileroVad.h"
#include <miniaudio.h>
#include <openvino/openvino.hpp>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <cstring>

#ifndef MYWAVE_ASSETS_DIR
#define MYWAVE_ASSETS_DIR ""
#endif

namespace {

QVector<float> resampleTo16k(const QVector<float>& in, uint32_t inRate) {
    if (inRate == 16000 || in.isEmpty()) return in;
    ma_resampler_config config = ma_resampler_config_init(
        ma_format_f32,
        1,
        inRate,
        16000,
        ma_resample_algorithm_linear
    );
    ma_resampler resampler;
    if (ma_resampler_init(&config, nullptr, &resampler) != MA_SUCCESS) {
        qWarning() << "[SileroVad] Failed to initialize miniaudio resampler!";
        return in;
    }
    ma_uint64 inFrames = static_cast<ma_uint64>(in.size());
    ma_uint64 outFrames = (inFrames * 16000 + inRate - 1) / inRate + 64;
    QVector<float> out(static_cast<qsizetype>(outFrames));
    ma_uint64 framesIn = inFrames;
    ma_uint64 framesOut = outFrames;
    ma_resampler_process_pcm_frames(&resampler, in.constData(), &framesIn, out.data(), &framesOut);
    out.resize(static_cast<qsizetype>(framesOut));
    ma_resampler_uninit(&resampler, nullptr);
    return out;
}

} // namespace

QString SileroVad::resolveModelPath() {
    static const QStringList candidates = {
        QStringLiteral(MYWAVE_ASSETS_DIR "/silero_vad_16k.xml"),
        QStringLiteral(MYWAVE_ASSETS_DIR "/silero_vad_16k.onnx"),
        QStringLiteral("assets/model/silero_vad_16k.xml"),
        QStringLiteral("assets/model/silero_vad_16k.onnx"),
        QStringLiteral("../assets/model/silero_vad_16k.xml"),
        QStringLiteral("../assets/model/silero_vad_16k.onnx"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/assets/model/silero_vad_16k.xml"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/assets/model/silero_vad_16k.onnx"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/../assets/model/silero_vad_16k.xml"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/../assets/model/silero_vad_16k.onnx"),
        QStringLiteral("silero-vad/src/silero_vad/data/silero_vad_openvino_16k.onnx"),
    };

    for (const auto& path : candidates) {
        if (!path.isEmpty() && QFileInfo::exists(path)) {
            return QFileInfo(path).canonicalFilePath();
        }
    }
    return QString();
}

bool SileroVad::isModelAvailable() {
    return !resolveModelPath().isEmpty();
}

std::shared_ptr<ov::CompiledModel> SileroVad::getCompiledModel() {
    static std::mutex s_mutex;
    static std::shared_ptr<ov::CompiledModel> s_compiled;
    static QString s_cachedPath;

    QString modelPath = resolveModelPath();
    if (modelPath.isEmpty()) {
        qWarning() << "[SileroVad] Could not find Silero VAD model in any known assets paths!";
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_compiled && s_cachedPath == modelPath) {
        return s_compiled;
    }

    try {
        ov::Core core;
        auto model = core.read_model(modelPath.toStdString());
        // Compile model with f32 precision hint (vital for Silero recurrent state accuracy)
        ov::CompiledModel compiled = core.compile_model(
            model, "CPU", ov::hint::inference_precision(ov::element::f32)
        );
        s_compiled = std::make_shared<ov::CompiledModel>(std::move(compiled));
        s_cachedPath = modelPath;
        qDebug() << "[SileroVad] Successfully compiled OpenVINO model from:" << modelPath;
        return s_compiled;
    } catch (const std::exception& ex) {
        qWarning() << "[SileroVad] Exception compiling OpenVINO model:" << ex.what();
        return nullptr;
    }
}

VadResult SileroVad::process(const QVector<float>& samples, uint32_t sampleRate,
                             int) const {
    VadResult result;
    if (samples.isEmpty() || sampleRate == 0) return result;

    auto compiledModel = getCompiledModel();
    if (!compiledModel) return result;

    const QVector<float> audio16k = (sampleRate == 16000)
        ? samples
        : resampleTo16k(samples, sampleRate);

    constexpr int kChunkSize = 512;
    constexpr int kContextSize = 64;
    constexpr int kEffectiveInputSize = kChunkSize + kContextSize; // 576
    constexpr int kStateSize = 2 * 1 * 128; // 256

    if (audio16k.size() < kChunkSize) return result;

    const int numChunks = static_cast<int>(audio16k.size() / kChunkSize);
    if (numChunks <= 0) return result;

    const uint32_t origHop = static_cast<uint32_t>(std::max(1.0, std::round(512.0 * double(sampleRate) / 16000.0)));
    result.frameSize = origHop;
    result.analysisFrameSize = origHop;
    result.energyLabel = QStringLiteral("语音概率 (Silero VAD)");
    result.zcrLabel = QStringLiteral("判定阈值");

    ov::InferRequest inferRequest;
    try {
        inferRequest = compiledModel->create_infer_request();
    } catch (const std::exception& ex) {
        qWarning() << "[SileroVad] Failed to create infer request:" << ex.what();
        return result;
    }

    ov::Tensor inputTensor(ov::element::f32, {1, static_cast<size_t>(kEffectiveInputSize)});
    ov::Tensor stateTensor(ov::element::f32, {2, 1, 128});

    float* inputPtr = inputTensor.data<float>();
    float* statePtr = stateTensor.data<float>();
    std::fill_n(statePtr, kStateSize, 0.0f);

    inferRequest.set_tensor("input", inputTensor);
    inferRequest.set_tensor("state", stateTensor);

    std::vector<float> context(kContextSize, 0.0f);
    QVector<float> speechProbs;
    speechProbs.reserve(numChunks);

    const float* pcm16k = audio16k.constData();

    for (int i = 0; i < numChunks; ++i) {
        std::copy_n(context.data(), kContextSize, inputPtr);
        std::copy_n(pcm16k + i * kChunkSize, kChunkSize, inputPtr + kContextSize);

        inferRequest.infer();

        float prob = inferRequest.get_tensor("output").data<float>()[0];
        speechProbs.append(prob);

        const float* nextState = inferRequest.get_tensor("stateN").data<float>();
        std::copy_n(nextState, kStateSize, statePtr);
        std::copy_n(inputPtr + 512, kContextSize, context.data());
    }

    const float negThreshold = std::max(threshold - negThresholdOffset, 0.01f);
    const int minSpeechSamples = 16000 * minSpeechMs / 1000;
    const int speechPadSamples = 16000 * speechPadMs / 1000;
    const int minSilenceSamples = 16000 * minSilenceMs / 1000;
    const int audioLengthSamples = numChunks * kChunkSize;

    struct Segment {
        int start = 0;
        int end = 0;
    };
    QVector<Segment> speeches;
    Segment currentSpeech{0, 0};
    bool triggered = false;
    int tempEnd = 0;

    for (int i = 0; i < speechProbs.size(); ++i) {
        float prob = speechProbs[i];
        int curSample = i * kChunkSize;

        if (prob >= threshold && tempEnd != 0) {
            tempEnd = 0;
        }

        if (prob >= threshold && !triggered) {
            triggered = true;
            currentSpeech.start = curSample;
            continue;
        }

        if (prob < negThreshold && triggered) {
            if (tempEnd == 0) {
                tempEnd = curSample;
            }
            int silDurNow = curSample - tempEnd;
            if (silDurNow < minSilenceSamples) {
                continue;
            } else {
                currentSpeech.end = tempEnd;
                if ((currentSpeech.end - currentSpeech.start) > minSpeechSamples) {
                    speeches.append(currentSpeech);
                }
                currentSpeech = {0, 0};
                tempEnd = 0;
                triggered = false;
                continue;
            }
        }
    }

    if (triggered && (audioLengthSamples - currentSpeech.start) > minSpeechSamples) {
        currentSpeech.end = audioLengthSamples;
        speeches.append(currentSpeech);
    }

    for (int i = 0; i < speeches.size(); ++i) {
        if (i == 0) {
            speeches[i].start = std::max(0, speeches[i].start - speechPadSamples);
        }
        if (i != speeches.size() - 1) {
            int silenceDuration = speeches[i + 1].start - speeches[i].end;
            if (silenceDuration < 2 * speechPadSamples) {
                speeches[i].end += silenceDuration / 2;
                speeches[i + 1].start = std::max(0, speeches[i + 1].start - silenceDuration / 2);
            } else {
                speeches[i].end = std::min(audioLengthSamples, speeches[i].end + speechPadSamples);
                speeches[i + 1].start = std::max(0, speeches[i + 1].start - speechPadSamples);
            }
        } else {
            speeches[i].end = std::min(audioLengthSamples, speeches[i].end + speechPadSamples);
        }
    }

    QVector<float> confidence(numChunks, 0.0f);
    for (const auto& sp : speeches) {
        int startChunk = std::max(0, sp.start / kChunkSize);
        int endChunk = std::min(numChunks, (sp.end + kChunkSize - 1) / kChunkSize);
        for (int c = startChunk; c < endChunk; ++c) {
            confidence[c] = 1.0f;
        }
    }

    result.confidence = std::move(confidence);
    result.energy = speechProbs;
    result.zcr.clear();
    result.highEnergyThreshold = threshold;
    result.lowEnergyThreshold = negThreshold;
    result.zcrThreshold = threshold;
    result.zcrLabel.clear();
    return result;
}

QVector<VadParamDef> SileroVad::parameters() const {
    return {
        VadParamDef::mkFloat("threshold", QStringLiteral("语音概率阈值"),
                             threshold, 0.05f, 0.95f, 0.05f),
        VadParamDef::mkFloat("neg_threshold_offset", QStringLiteral("滞后门限偏移"),
                             negThresholdOffset, 0.0f, 0.35f, 0.01f),
        VadParamDef::mkInt("min_speech_ms", QStringLiteral("最小语音时长 (ms)"),
                           minSpeechMs, 30, 1000),
        VadParamDef::mkInt("min_silence_ms", QStringLiteral("最小静音时长 (ms)"),
                           minSilenceMs, 30, 1000),
        VadParamDef::mkInt("speech_pad_ms", QStringLiteral("语音边缘扩展 (ms)"),
                           speechPadMs, 0, 200),
    };
}

void SileroVad::setParameter(const QString& key, float value) {
    if (key == QLatin1String("threshold"))
        threshold = std::clamp(value, 0.05f, 0.95f);
    else if (key == QLatin1String("neg_threshold_offset"))
        negThresholdOffset = std::clamp(value, 0.0f, 0.35f);
    else if (key == QLatin1String("min_speech_ms"))
        minSpeechMs = std::clamp(static_cast<int>(value), 30, 1000);
    else if (key == QLatin1String("min_silence_ms"))
        minSilenceMs = std::clamp(static_cast<int>(value), 30, 1000);
    else if (key == QLatin1String("speech_pad_ms"))
        speechPadMs = std::clamp(static_cast<int>(value), 0, 200);
}
