#include "AudioEngine.h"
#include "../dsp/DspUtils.h"
#include "../dsp/FftProcessor.h"
#include "../dsp/EnergyCalculator.h"
#include "../dsp/ZeroCrossingRate.h"
#include "../dsp/LpcProcessor.h"
#include "../dsp/PitchDetector.h"
#include "../dsp/FeatureExtractor.h"
#include "../vad/CepstralVad.h"
#include "Parallel.h"
#include "DownSampler.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <miniaudio.h>

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
    , m_decoder()
{
    connect(&m_player, &AudioPlayer::playbackStateChanged,
            this, [this](const PlaybackState& ps, float chartPos) {
        ChartEvent ev;
        ev.type = ChartEvent::UpdatePlaybackState;
        ev.playbackState = ps;
        ev.chartPosition = chartPos;
        emit chartEvent(ev);
    });
}

// ── Configuration ───────────────────────────────────────────────────────────

void AudioEngine::setConfig(const EngineConfig& config) {
    int oldFrameSize = m_config.frameSize;
    m_config = config;
    if (oldFrameSize != config.frameSize && oldFrameSize > 0) {
        // Recompute all non-Audio charts
        auto allCharts = m_cache.getAll();
        for (const auto& [fp, chart] : allCharts) {
            if (chart.dataType != DataType::Audio)
                m_cache.remove(fp, chart.dataType);
        }
        for (const auto& [fp, chart] : allCharts) {
            if (chart.dataType != DataType::Audio)
                recomputeChartIntoCache(fp, chart.dataType);
        }
    }
    recomputeRanges();
    m_indexRangeStart = 0.0f;
    m_indexRangeEnd = m_maxIndex;
    syncToUi();
    if (!m_vadDebug.isEmpty()) {
        ChartEvent debugEvent;
        debugEvent.type = ChartEvent::UpdateVadDebug;
        debugEvent.vadDebug = m_vadDebug.constBegin().value();
        emit chartEvent(debugEvent);
    }
}

void AudioEngine::setDownSamplePointsNum(int n) {
    m_downSamplePointsNum = std::max(100, n);
    syncToUi();
}

void AudioEngine::setIndexRange(float start, float end) {
    m_indexRangeStart = start;
    m_indexRangeEnd = end;
    syncToUi();
}

// ── Audio file management ───────────────────────────────────────────────────

void AudioEngine::addFile(const QString& filePath, const QByteArray& fileData,
                           const QString& format) {
    qDebug() << "[ENGINE] addFile:" << filePath << format << fileData.size() << "bytes";

    auto decoded = m_decoder.decode(filePath, fileData, format);
    decoded.info.filePath = filePath;
    decoded.info.format = format;
    if (decoded.samples.isEmpty() || decoded.info.sampleRate == 0) return;

    qDebug() << "[ENGINE] decoded:" << decoded.samples.size() << "samples,"
             << decoded.info.sampleRate << "Hz," << decoded.info.channels << "ch";

    m_storage.save(filePath, decoded);

    // Create Audio waveform chart
    const auto& samples = decoded.samples;
    QVector<ChartPoint> points(samples.size());
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < samples.size(); ++i) {
        points[i] = {float(i), samples[i]};
        minY = std::min(minY, samples[i]);
        maxY = std::max(maxY, samples[i]);
    }

    CachedChart audioChart;
    audioChart.dataType = DataType::Audio;
    audioChart.points = std::move(points);
    audioChart.minY = minY;
    audioChart.maxY = maxY;
    audioChart.visible = true;

    m_cache.add(filePath, audioChart);

    recomputeRanges();
    m_indexRangeStart = 0.0f;
    m_indexRangeEnd = m_maxIndex;
    emitRangeSignals();

    // Keep the full waveform in cache and reduce it only for UI delivery.
    ChartEvent ev;
    ev.type = ChartEvent::AddChart;
    ev.key = filePath;
    ev.dataType = DataType::Audio;
    ev.chart = prepareChartForUi(audioChart);
    emit chartEvent(ev);
}

void AudioEngine::addChart(const QString& filePath, DataType dataType) {
    qDebug() << "[ENGINE] addChart:" << filePath << dataTypeName(dataType);

    auto cached = m_cache.get(filePath, dataType);
    if (cached.has_value()) {
        recomputeRanges();
        emitRangeSignals();

        ChartEvent ev;
        ev.type = ChartEvent::AddChart;
        ev.key = filePath;
        ev.dataType = dataType;
        ev.chart = prepareChartForUi(*cached);
        emit chartEvent(ev);

        if (dataType == DataType::Vad) {
            std::lock_guard<std::mutex> lock(m_vadDebugMutex);
            if (m_vadDebug.contains(filePath)) {
                ChartEvent debugEvent;
                debugEvent.type = ChartEvent::UpdateVadDebug;
                debugEvent.vadDebug = m_vadDebug.value(filePath);
                emit chartEvent(debugEvent);
            }
        }
        return;
    }

    (void)QtConcurrent::run([this, filePath, dataType]() {
        recomputeChartIntoCache(filePath, dataType);
        QMetaObject::invokeMethod(this, [this, filePath, dataType]() {
            recomputeRanges();
            emitRangeSignals();

            auto chart = m_cache.get(filePath, dataType);
            if (!chart) return;

            ChartEvent ev;
            ev.type = ChartEvent::AddChart;
            ev.key = filePath;
            ev.dataType = dataType;
            ev.chart = prepareChartForUi(*chart);
            emit chartEvent(ev);

            if (dataType == DataType::Vad) {
                std::lock_guard<std::mutex> lock(m_vadDebugMutex);
                if (m_vadDebug.contains(filePath)) {
                    ChartEvent debugEvent;
                    debugEvent.type = ChartEvent::UpdateVadDebug;
                    debugEvent.vadDebug = m_vadDebug.value(filePath);
                    emit chartEvent(debugEvent);
                }
            }
        }, Qt::QueuedConnection);
    });
}

void AudioEngine::removeChart(const QString& filePath, DataType dataType) {
    m_cache.remove(filePath, dataType);
    if (dataType == DataType::Vad) {
        m_vadDebug.remove(filePath);
        ChartEvent debugEvent;
        debugEvent.type = ChartEvent::UpdateVadDebug;
        emit chartEvent(debugEvent);
    }

    ChartEvent ev;
    ev.type = ChartEvent::RemoveChart;
    ev.key = filePath;
    ev.dataType = dataType;
    emit chartEvent(ev);

    recomputeRanges();
    emitRangeSignals();
}

void AudioEngine::setChartVisibility(const QString& filePath, DataType dataType,
                                      bool visible) {
    auto chart = m_cache.get(filePath, dataType);
    if (!chart) return;
    chart->visible = visible;
    m_cache.add(filePath, *chart);
    syncToUi();
}

void AudioEngine::removeAudio(const QString& filePath) {
    m_cache.removeAll(filePath);
    m_storage.remove(filePath);
    const bool removedVadDebug = m_vadDebug.remove(filePath) > 0;

    ChartEvent ev;
    ev.type = ChartEvent::RemoveAllCharts;
    emit chartEvent(ev);
    if (removedVadDebug) {
        ChartEvent debugEvent;
        debugEvent.type = ChartEvent::UpdateVadDebug;
        emit chartEvent(debugEvent);
    }
}

AudioInfo AudioEngine::getAudioInfo(const QString& filePath) const {
    auto audio = m_storage.load(filePath);
    if (audio) return audio->info;
    return {};
}

QStringList AudioEngine::loadedFiles() const {
    return m_cache.filePaths();
}

bool AudioEngine::saveWavSegment(const QString& filePath, const QString& outputPath,
                                 uint64_t firstSample, uint64_t lastSample) const {
    const auto audio = m_storage.load(filePath);
    if (!audio || audio->samples.isEmpty() || audio->info.sampleRate == 0) return false;
    firstSample = std::min<uint64_t>(firstSample, audio->samples.size());
    lastSample = std::min<uint64_t>(lastSample, audio->samples.size());
    if (lastSample <= firstSample) return false;

    const bool stereo = audio->info.channels >= 2
        && audio->leftSamples.size() == audio->samples.size()
        && audio->rightSamples.size() == audio->samples.size();
    const ma_uint32 channels = stereo ? 2 : 1;

    ma_encoder_config config = ma_encoder_config_init(
        ma_encoding_format_wav,
        ma_format_s16,
        channels,
        static_cast<ma_uint32>(audio->info.sampleRate)
    );
    ma_encoder encoder;
    if (ma_encoder_init_file(outputPath.toUtf8().constData(), &config, &encoder) != MA_SUCCESS) {
        return false;
    }

    const qsizetype frames = qsizetype(lastSample - firstSample);
    QVector<float> floatData(frames * channels);
    for (qsizetype i = 0; i < frames; ++i) {
        const qsizetype source = qsizetype(firstSample) + i;
        if (stereo) {
            floatData[i * 2]     = audio->leftSamples[source];
            floatData[i * 2 + 1] = audio->rightSamples[source];
        } else {
            floatData[i]         = audio->samples[source];
        }
    }
    QVector<ma_int16> s16Data(frames * channels);
    ma_pcm_f32_to_s16(s16Data.data(), floatData.constData(), frames * channels, ma_dither_mode_none);

    ma_uint64 framesWritten = 0;
    const ma_result writeRes = ma_encoder_write_pcm_frames(&encoder, s16Data.constData(), frames, &framesWritten);
    ma_encoder_uninit(&encoder);
    return writeRes == MA_SUCCESS && framesWritten == static_cast<ma_uint64>(frames);
}

bool AudioEngine::concatenate(const QString& firstFile, const QString& secondFile,
                              const QString& outputPath) const {
    const auto first = m_storage.load(firstFile);
    const auto second = m_storage.load(secondFile);
    if (!first || !second || first->info.sampleRate != second->info.sampleRate)
        return false;

    ma_encoder_config config = ma_encoder_config_init(
        ma_encoding_format_wav,
        ma_format_s16,
        1,
        static_cast<ma_uint32>(first->info.sampleRate)
    );
    ma_encoder encoder;
    if (ma_encoder_init_file(outputPath.toUtf8().constData(), &config, &encoder) != MA_SUCCESS) {
        return false;
    }

    QVector<ma_int16> s16First(first->samples.size());
    ma_pcm_f32_to_s16(s16First.data(), first->samples.constData(), first->samples.size(), ma_dither_mode_none);
    ma_uint64 written1 = 0;
    const ma_result res1 = ma_encoder_write_pcm_frames(&encoder, s16First.constData(), first->samples.size(), &written1);

    QVector<ma_int16> s16Second(second->samples.size());
    ma_pcm_f32_to_s16(s16Second.data(), second->samples.constData(), second->samples.size(), ma_dither_mode_none);
    ma_uint64 written2 = 0;
    const ma_result res2 = ma_encoder_write_pcm_frames(&encoder, s16Second.constData(), second->samples.size(), &written2);

    ma_encoder_uninit(&encoder);
    return res1 == MA_SUCCESS && res2 == MA_SUCCESS
        && written1 == static_cast<ma_uint64>(first->samples.size())
        && written2 == static_cast<ma_uint64>(second->samples.size());
}

bool AudioEngine::exportEffectiveMfcc(const QString& filePath,
                                      const QString& outputPath) const {
    const auto audio = m_storage.load(filePath);
    if (!audio) return false;
    ReferenceVad detector;
    const auto vad = detector.process(audio->samples, audio->info.sampleRate, m_config.frameSize);
    const auto frames = frameSignalFast(audio->samples, m_config.frameSize, true, true, 0.95f);
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;
    QTextStream stream(&file);
    stream << "frame";
    for (int coefficient = 0; coefficient < 13; ++coefficient)
        stream << '\t' << "mfcc_" << coefficient;
    stream << '\n';
    const int count = std::min<int>(frames.size(), static_cast<int>(vad.confidence.size()));
    FftProcessor fft(m_config.frameSize);
    for (int i = 0; i < count; ++i) {
        if (vad.confidence[i] < 0.5f) continue;
        const auto mfcc = fft.computeMFCC(frames.frame(i), m_config.frameSize, audio->info.sampleRate, 24, 13);
        stream << i;
        for (float value : mfcc) stream << '\t' << value;
        stream << '\n';
    }
    return stream.status() == QTextStream::Ok;
}

// ── VAD ─────────────────────────────────────────────────────────────────────

QStringList AudioEngine::listVadAlgorithms() const { return m_vad.listAlgorithms(); }
QString AudioEngine::currentVadName() const { return m_vad.currentName(); }

void AudioEngine::setVadAlgorithm(const QString& name) {
    m_vad.setAlgorithm(name);
    m_cache.removeByDataType(DataType::Vad);
    m_vadDebug.clear();
    syncToUi();

    ChartEvent debugEvent;
    debugEvent.type = ChartEvent::UpdateVadDebug;
    emit chartEvent(debugEvent);
}

QVector<VadParamDef> AudioEngine::vadParams() const { return m_vad.parameters(); }

void AudioEngine::setVadParam(const QString& key, float value) {
    m_vad.setParameter(key, value);
}

void AudioEngine::runVad(const QString& filePath) {
    if (!m_storage.load(filePath)) return;
    m_cache.remove(filePath, DataType::Vad);

    (void)QtConcurrent::run([this, filePath]() {
        recomputeChartIntoCache(filePath, DataType::Vad);
        QMetaObject::invokeMethod(this, [this, filePath]() {
            recomputeRanges();
            emitRangeSignals();

            const auto chart = m_cache.get(filePath, DataType::Vad);
            if (chart) {
                ChartEvent chartEventValue;
                chartEventValue.type = ChartEvent::AddChart;
                chartEventValue.key = filePath;
                chartEventValue.dataType = DataType::Vad;
                chartEventValue.chart = prepareChartForUi(*chart);
                emit chartEvent(chartEventValue);
            }
            std::lock_guard<std::mutex> lock(m_vadDebugMutex);
            if (m_vadDebug.contains(filePath)) {
                ChartEvent debugEvent;
                debugEvent.type = ChartEvent::UpdateVadDebug;
                debugEvent.vadDebug = m_vadDebug.value(filePath);
                emit chartEvent(debugEvent);
            }
        }, Qt::QueuedConnection);
    });
}

// ── Audio playback ───────────────────────────────────────────────────────────

void AudioEngine::playAudio(const QString& filePath, double startFraction) {
    auto audio = m_storage.load(filePath);
    if (!audio) return;
    m_player.load(*audio);
    uint64_t total = audio->samples.size();
    uint64_t start = std::min(total, (uint64_t)(total * startFraction));
    m_player.play(start);
}

void AudioEngine::playAudioRange(const QString& filePath, uint64_t firstSample,
                                 uint64_t lastSample) {
    const auto audio = m_storage.load(filePath);
    if (!audio) return;
    m_player.load(*audio);
    m_player.play(firstSample, lastSample);
}

void AudioEngine::resumeAudio() { m_player.resume(); }
void AudioEngine::pauseAudio() { m_player.pause(); }
void AudioEngine::stopAudio() { m_player.stop(); }

void AudioEngine::seekAudio(double fraction) {
    auto total = m_player.totalSamples();
    if (total)
        m_player.seek((uint64_t)(*total * fraction));
}

void AudioEngine::setPlaybackSpeed(float m) { m_player.setSpeed(m); }

// ── Internal helpers ─────────────────────────────────────────────────────────

void AudioEngine::recomputeRanges() {
    m_maxIndex = 10000.0f;
    m_yMin = -0.5f;
    m_yMax = 0.5f;

    auto visible = m_cache.getVisible();
    float yMin = std::numeric_limits<float>::max();
    float yMax = std::numeric_limits<float>::lowest();
    float maxIdx = 0.0f;
    bool found = false;

    for (const auto& [key, chart] : visible) {
        if (chart.dataType == DataType::Spectrogram) {
            maxIdx = std::max(maxIdx, chart.heatXMax);
            continue;
        }
        found = true;
        yMin = std::min(yMin, chart.minY);
        yMax = std::max(yMax, chart.maxY);
        if (!chart.points.isEmpty())
            maxIdx = std::max(maxIdx, chart.points.last().x);
    }

    if (found && yMin <= yMax) {
        m_yMin = yMin;
        m_yMax = yMax;
    }
    if (maxIdx > 0.0f)
        m_maxIndex = std::ceil(maxIdx / m_config.frameSize) * m_config.frameSize;
}

void AudioEngine::syncToUi() {
    recomputeRanges();
    emitRangeSignals();

    const auto visible = m_cache.getVisible();
    QVector<QPair<QString, CachedChart>> chartsForUi;
    chartsForUi.reserve(visible.size());
    for (const auto& [key, chart] : visible) {
        chartsForUi.append({key, prepareChartForUi(chart)});
    }

    ChartEvent ev;
    ev.type = ChartEvent::UpdateAllCharts;
    ev.chartList = std::move(chartsForUi);
    emit chartEvent(ev);
}

CachedChart AudioEngine::prepareChartForUi(const CachedChart& chart) const {
    // The cache always owns the full-resolution result. Range clipping and
    // min/max reduction happen exactly once, immediately before UI delivery.
    const CachedChart ranged = isFullRange()
        ? chart
        : chart.getRange(m_indexRangeStart, m_indexRangeEnd);
    return minMaxDownsample(ranged, m_downSamplePointsNum);
}

void AudioEngine::emitRangeSignals() {
    ChartEvent evMax;
    evMax.type = ChartEvent::UpdateMaxIndex;
    evMax.maxIndex = m_maxIndex;
    emit chartEvent(evMax);

    ChartEvent evY;
    evY.type = ChartEvent::UpdateYRange;
    evY.minY = m_yMin;
    evY.maxY = m_yMax;
    emit chartEvent(evY);
}

bool AudioEngine::isFullRange() const {
    return m_indexRangeStart <= 0.0f && m_indexRangeEnd >= m_maxIndex;
}

void AudioEngine::recomputeChartIntoCache(const QString& filePath, DataType dt) {
    auto audio = m_storage.load(filePath);
    if (!audio) return;

    CachedChart chart;
    chart.dataType = dt;

    switch (dt) {
    case DataType::Spectrum:
    case DataType::PowerSpectrum:
    case DataType::LogSpectrum:
    case DataType::Cepstrum:
    case DataType::MelSpectrum: {
        chart = FeatureExtractor::spectrum(audio->samples, m_config.frameSize,
                                           dt, audio->info.sampleRate);
        break;
    }
    case DataType::Energy: {
        chart = computeEnergy(audio->samples, m_config.frameSize);
        break;
    }
    case DataType::ZeroCrossingRate: {
        chart = computeZCR(audio->samples, m_config.frameSize);
        break;
    }
    case DataType::Vad: {
        auto result = m_vad.process(audio->samples, audio->info.sampleRate,
                                     m_config.frameSize);
        QVector<ChartPoint> pts(result.confidence.size());
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        for (int i = 0; i < result.confidence.size(); ++i) {
            float v = result.confidence[i];
            const float center = float(i * result.frameSize)
                + 0.5f * float(result.analysisFrameSize);
            pts[i] = {center, v};
            minY = std::min(minY, v);
            maxY = std::max(maxY, v);
        }
        chart.points = std::move(pts);
        chart.minY = minY;
        chart.maxY = maxY;

        VadDebugData debug;
        debug.filePath = filePath;
        debug.algorithm = m_vad.currentName();
        debug.energyLabel = result.energyLabel.isEmpty()
            ? QStringLiteral("VAD Energy") : result.energyLabel;
        debug.zcrLabel = result.zcrLabel.isEmpty()
            ? QStringLiteral("VAD ZCR") : result.zcrLabel;
        debug.frameSize = int(result.analysisFrameSize);
        debug.hopSize = int(result.frameSize);
        debug.lowEnergyThreshold = result.lowEnergyThreshold;
        debug.highEnergyThreshold = result.highEnergyThreshold;
        debug.zcrThreshold = result.zcrThreshold;
        debug.energy.reserve(result.energy.size());
        debug.zcr.reserve(result.zcr.size());
        debug.decision.reserve(result.confidence.size());
        for (int i = 0; i < result.energy.size(); ++i)
            debug.energy.append({float(i * result.frameSize)
                                     + 0.5f * float(result.analysisFrameSize),
                                 result.energy[i]});
        for (int i = 0; i < result.zcr.size(); ++i)
            debug.zcr.append({float(i * result.frameSize)
                                  + 0.5f * float(result.analysisFrameSize),
                              result.zcr[i]});
        for (int i = 0; i < result.confidence.size(); ++i)
            debug.decision.append({float(i * result.frameSize)
                                       + 0.5f * float(result.analysisFrameSize),
                                   result.confidence[i]});
        {
            std::lock_guard<std::mutex> lock(m_vadDebugMutex);
            m_vadDebug[filePath] = std::move(debug);
        }
        break;
    }
    case DataType::AvgAmplitude: {
        const int frameSize = m_config.frameSize;
        const int hopSize = frameSize / 2;
        if (audio->samples.size() >= frameSize && frameSize > 0) {
            const int nFrames = (audio->samples.size() - frameSize) / hopSize + 1;
            QVector<ChartPoint> pts(nFrames);
            float minY = 1e30f, maxY = -1e30f;
            const float* src = audio->samples.constData();
            for (int i = 0; i < nFrames; ++i) {
                float v = frameAvgAmplitude(src + i * hopSize, frameSize);
                pts[i] = {float(i * hopSize), v};
                minY = std::min(minY, v); maxY = std::max(maxY, v);
            }
            chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        }
        break;
    }
    case DataType::SpectrumFFT: {
        chart = FeatureExtractor::spectrum(audio->samples, m_config.frameSize,
                                           dt, audio->info.sampleRate);
        break;
    }
    case DataType::AutoCorrelation: {
        auto frames = frameSignalFast(audio->samples, m_config.frameSize, false, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        pts.reserve(frames.size() * 14);
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            auto acf = autoCorrelation(frames.frame(i), m_config.frameSize, 13);
            for (int j = 0; j < acf.size(); ++j) {
                pts.append({float(i * hopSize + j), acf[j]});
                minY = std::min(minY, acf[j]); maxY = std::max(maxY, acf[j]);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Lpc:
    case DataType::Lpcc:
    case DataType::Mfcc: {
        chart = FeatureExtractor::coefficients(audio->samples, m_config.frameSize,
                                               dt, audio->info.sampleRate);
        break;
    }
    case DataType::LpcReconstructed: {
        chart = FeatureExtractor::reconstructLpc(audio->samples, m_config.frameSize);
        break;
    }
    case DataType::PitchAcf:
    case DataType::PitchAmdf:
    case DataType::PitchCep: {
        PitchResult pr;
        if (dt == DataType::PitchAcf)
            pr = detectPitchACF(audio->samples, m_config.frameSize, audio->info.sampleRate);
        else if (dt == DataType::PitchAmdf)
            pr = detectPitchAMDF(audio->samples, m_config.frameSize, audio->info.sampleRate);
        else
            pr = detectPitchCepstral(audio->samples, m_config.frameSize, audio->info.sampleRate);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts(pr.pitch.size());
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < pr.pitch.size(); ++i) {
            pts[i] = {float(i * hopSize), pr.pitch[i]};
            minY = std::min(minY, pr.pitch[i]); maxY = std::max(maxY, pr.pitch[i]);
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::CepstralVad: {
        CepstralVad cv;
        auto result = cv.process(audio->samples, audio->info.sampleRate, m_config.frameSize);
        QVector<ChartPoint> pts(result.confidence.size());
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < result.confidence.size(); ++i) {
            pts[i] = {float(i * result.frameSize)
                          + 0.5f * float(result.analysisFrameSize),
                      result.confidence[i]};
            minY = std::min(minY, result.confidence[i]); maxY = std::max(maxY, result.confidence[i]);
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Spectrogram: {
        chart = FeatureExtractor::spectrogram(audio->samples, m_config.frameSize,
                                              audio->info.sampleRate);
        break;
    }
    case DataType::AudioLeft:
    case DataType::AudioRight: {
        const auto& samples = dt == DataType::AudioLeft
            ? audio->leftSamples : audio->rightSamples;
        if (samples.isEmpty()) break;
        chart.points.resize(samples.size());
        chart.minY = std::numeric_limits<float>::max();
        chart.maxY = std::numeric_limits<float>::lowest();
        for (int i = 0; i < samples.size(); ++i) {
            chart.points[i] = {float(i), samples[i]};
            chart.minY = std::min(chart.minY, samples[i]);
            chart.maxY = std::max(chart.maxY, samples[i]);
        }
        break;
    }
    case DataType::Audio:
        return; // Already in cache
    }
    chart.visible = true;
    m_cache.add(filePath, chart);
}
