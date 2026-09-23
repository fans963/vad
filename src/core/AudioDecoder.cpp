#include "AudioDecoder.h"

#include <miniaudio.h>
#include <QFileInfo>
#include <QDebug>

static AudioData processDecodedFrames(const QString& filePath,
                                      const QString& formatHint,
                                      ma_uint32 channels,
                                      ma_uint32 sampleRate,
                                      ma_uint64 totalFrames,
                                      const float* pFrames)
{
    AudioData result;
    if (!pFrames || totalFrames == 0 || sampleRate == 0 || channels == 0) {
        return result;
    }

    const auto frameCount = static_cast<qsizetype>(totalFrames);
    result.samples.resize(frameCount);
    if (channels >= 1) result.leftSamples.resize(frameCount);
    if (channels >= 2) result.rightSamples.resize(frameCount);

    for (qsizetype i = 0; i < frameCount; ++i) {
        float sum = 0.0f;
        for (ma_uint32 ch = 0; ch < channels; ++ch) {
            sum += pFrames[i * channels + ch];
        }
        result.samples[i] = sum / static_cast<float>(channels);
        result.leftSamples[i] = pFrames[i * channels];
        if (channels >= 2) {
            result.rightSamples[i] = pFrames[i * channels + 1];
        }
    }

    result.info.filePath = filePath;
    QString fmt = formatHint;
    if (fmt.isEmpty() && !filePath.isEmpty()) {
        fmt = QFileInfo(filePath).suffix().toLower();
    }
    result.info.format = fmt;
    result.info.sampleRate = static_cast<int>(sampleRate);
    result.info.channels = static_cast<int>(channels);
    result.info.sampleCount = result.samples.size();
    if (sampleRate > 0) {
        result.info.durationSecs = static_cast<float>(result.samples.size()) / static_cast<float>(sampleRate);
    }

    qDebug() << "[Decoder] miniaudio done:" << result.samples.size() << "samples,"
             << result.info.sampleRate << "Hz," << channels << "ch";
    return result;
}

AudioData AudioDecoder::decode(const QString& filePath,
                               const QByteArray& rawData,
                               const QString& format)
{
    if (rawData.isEmpty()) {
        if (!filePath.isEmpty()) {
            return decodeFile(filePath);
        }
        return {};
    }

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    ma_uint64 totalFrames = 0;
    float* pFrames = nullptr;
    ma_result res = ma_decode_memory(rawData.constData(), rawData.size(), &config, &totalFrames, reinterpret_cast<void**>(&pFrames));
    if (res != MA_SUCCESS || !pFrames) {
        qWarning() << "[Decoder] ma_decode_memory failed with code:" << res;
        return {};
    }

    AudioData result = processDecodedFrames(filePath, format, config.channels, config.sampleRate, totalFrames, pFrames);
    ma_free(pFrames, nullptr);
    return result;
}

AudioData AudioDecoder::decodeFile(const QString& filePath)
{
    qDebug() << "[Decoder] miniaudio decodeFile:" << filePath;

    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
    ma_uint64 totalFrames = 0;
    float* pFrames = nullptr;
    ma_result res = ma_decode_file(filePath.toUtf8().constData(), &config, &totalFrames, reinterpret_cast<void**>(&pFrames));
    if (res != MA_SUCCESS || !pFrames) {
        qWarning() << "[Decoder] ma_decode_file failed for:" << filePath << "with code:" << res;
        return {};
    }

    AudioData result = processDecodedFrames(filePath, QString(), config.channels, config.sampleRate, totalFrames, pFrames);
    ma_free(pFrames, nullptr);
    return result;
}
