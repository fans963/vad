#include "AudioEngine.h"
#include "../dsp/DspUtils.h"
#include "../dsp/FftProcessor.h"
#include "../dsp/EnergyCalculator.h"
#include "../dsp/ZeroCrossingRate.h"
#include "../dsp/LpcProcessor.h"
#include "../dsp/PitchDetector.h"
#include "../vad/CepstralVad.h"
#include "Parallel.h"
#include "DownSampler.h"

#include <QDebug>

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

    // Send AddChart for the Audio waveform
    auto ranged = audioChart.getRange(0.0f, m_maxIndex);
    auto ds = minMaxDownsample(ranged, m_downSamplePointsNum);

    ChartEvent ev;
    ev.type = ChartEvent::AddChart;
    ev.key = filePath;
    ev.dataType = DataType::Audio;
    ev.chart = ds;
    emit chartEvent(ev);
}

void AudioEngine::addChart(const QString& filePath, DataType dataType) {
    qDebug() << "[ENGINE] addChart:" << filePath << dataTypeName(dataType);

    // Check if already cached
    auto cached = m_cache.get(filePath, dataType);
    if (cached.has_value()) {
        // Just re-send
    } else {
        recomputeChartIntoCache(filePath, dataType);
    }

    recomputeRanges();
    emitRangeSignals();

    auto chart = m_cache.get(filePath, dataType);
    if (!chart) return;

    auto ranged = isFullRange() ? *chart : chart->getRange(m_indexRangeStart, m_indexRangeEnd);
    auto ds = minMaxDownsample(ranged, m_downSamplePointsNum);

    ChartEvent ev;
    ev.type = ChartEvent::AddChart;
    ev.key = filePath;
    ev.dataType = dataType;
    ev.chart = ds;
    emit chartEvent(ev);
}

void AudioEngine::removeChart(const QString& filePath, DataType dataType) {
    m_cache.remove(filePath, dataType);

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

    ChartEvent ev;
    ev.type = ChartEvent::RemoveAllCharts;
    emit chartEvent(ev);
}

AudioInfo AudioEngine::getAudioInfo(const QString& filePath) const {
    auto audio = m_storage.load(filePath);
    if (audio) return audio->info;
    return {};
}

QStringList AudioEngine::loadedFiles() const {
    return m_cache.filePaths();
}

// ── VAD ─────────────────────────────────────────────────────────────────────

QStringList AudioEngine::listVadAlgorithms() const { return m_vad.listAlgorithms(); }
QString AudioEngine::currentVadName() const { return m_vad.currentName(); }

void AudioEngine::setVadAlgorithm(const QString& name) {
    m_vad.setAlgorithm(name);
    m_cache.removeByDataType(DataType::Vad);
    syncToUi();
}

QVector<VadParamDef> AudioEngine::vadParams() const { return m_vad.parameters(); }

void AudioEngine::setVadParam(const QString& key, float value) {
    m_vad.setParameter(key, value);
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
        found = true;
        yMin = std::min(yMin, chart.minY);
        yMax = std::max(yMax, chart.maxY);
        if (!chart.points.isEmpty())
            maxIdx = std::max(maxIdx, chart.points.last().x);
    }

    if (found && yMin <= yMax) {
        m_yMin = yMin;
        m_yMax = yMax;
        m_maxIndex = std::ceil(maxIdx / m_config.frameSize) * m_config.frameSize;
    }
}

void AudioEngine::syncToUi() {
    recomputeRanges();
    emitRangeSignals();

    if (isFullRange()) {
        // Send all visible charts
        auto visible = m_cache.getVisible();
        QVector<QPair<QString, CachedChart>> dsCharts;
        for (const auto& [key, chart] : visible) {
            auto ds = minMaxDownsample(chart, m_downSamplePointsNum);
            dsCharts.append({key, ds});
        }

        ChartEvent ev;
        ev.type = ChartEvent::UpdateAllCharts;
        ev.chartList = dsCharts;
        emit chartEvent(ev);
    } else {
        // Send ranged charts
        auto visible = m_cache.getVisible();
        QVector<QPair<QString, CachedChart>> dsCharts;
        for (const auto& [key, chart] : visible) {
            auto ranged = chart.getRange(m_indexRangeStart, m_indexRangeEnd);
            auto ds = minMaxDownsample(ranged, m_downSamplePointsNum);
            dsCharts.append({key, ds});
        }

        ChartEvent ev;
        ev.type = ChartEvent::UpdateAllCharts;
        ev.chartList = dsCharts;
        emit chartEvent(ev);
    }
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
    case DataType::Spectrum: {
        auto& samples = audio->samples;
        int hopSize = m_config.frameSize / 2;
        int nFrames = (samples.size() - m_config.frameSize) / hopSize + 1;
        if (nFrames <= 0) break;

        // Pre-build windowed frames (parallel for speed)
        QVector<QVector<float>> frames(nFrames);
        parallelFor(nFrames, [&](int f) {
            int start = f * hopSize;
            QVector<float> frame(m_config.frameSize);
            for (int j = 0; j < m_config.frameSize; ++j) {
                float w = 0.54f - 0.46f * std::cos(2.0f * M_PI * j / (m_config.frameSize - 1));
                frame[j] = samples[start + j] * w;
            }
            frames[f] = std::move(frame);
        });

        // Per-frame parallel FFT — each thread gets its own FftProcessor+plan
        auto allMags = parallelMap(frames, [&](const QVector<float>& frame) {
            FftProcessor fft(m_config.frameSize);
            return fft.computeMagnitudeSpectrum(frame);
        });

        // Concatenate
        QVector<ChartPoint> pts;
        pts.reserve(nFrames * hopSize);
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();

        for (int f = 0; f < nFrames; ++f) {
            const auto& mags = allMags[f];
            int limit = std::min((int)mags.size(), hopSize);
            for (int j = 0; j < limit; ++j) {
                float y = mags[j];
                pts.append({float(f * hopSize + j), y});
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
        chart.points = std::move(pts);
        chart.minY = minY;
        chart.maxY = maxY;
        break;
    }
    case DataType::Energy: {
        chart = computeEnergy(audio->samples, m_config.frameSize,
                               m_downSamplePointsNum);
        break;
    }
    case DataType::ZeroCrossingRate: {
        chart = computeZCR(audio->samples, m_config.frameSize,
                            m_downSamplePointsNum);
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
            pts[i] = {float(i * result.frameSize), v};
            minY = std::min(minY, v);
            maxY = std::max(maxY, v);
        }
        chart.points = std::move(pts);
        chart.minY = minY;
        chart.maxY = maxY;
        break;
    }
    case DataType::AvgAmplitude: {
        auto frames = frameSignal(audio->samples, m_config.frameSize, false, false);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts(frames.size());
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            float v = frameAvgAmplitude(frames[i].constData(), m_config.frameSize);
            pts[i] = {float(i * hopSize), v};
            minY = std::min(minY, v); maxY = std::max(maxY, v);
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::SpectrumFFT: {
        // FFT of magnitude spectrum — compute Spectrum first, then FFT each frame
        auto frames = frameSignal(audio->samples, m_config.frameSize, true, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            FftProcessor fft1(m_config.frameSize);
            auto mags = fft1.computeMagnitudeSpectrum(frames[i]);
            FftProcessor fft2(mags.size());
            auto specFft = fft2.computeMagnitudeSpectrum(mags);
            int limit = std::min((int)specFft.size(), hopSize);
            for (int j = 0; j < limit; ++j) {
                float y = specFft[j];
                pts.append({float(i * hopSize + j), y});
                minY = std::min(minY, y); maxY = std::max(maxY, y);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::AutoCorrelation: {
        auto frames = frameSignal(audio->samples, m_config.frameSize, false, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            auto acf = autoCorrelation(frames[i], 13);
            for (int j = 0; j < acf.size(); ++j) {
                pts.append({float(i * hopSize + j), acf[j]});
                minY = std::min(minY, acf[j]); maxY = std::max(maxY, acf[j]);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Lpc: {
        auto frames = frameSignal(audio->samples, m_config.frameSize, true, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            auto lpc = computeLPC(frames[i], 13);
            for (int j = 0; j < lpc.size(); ++j) {
                pts.append({float(i * hopSize + j), lpc[j]});
                minY = std::min(minY, lpc[j]); maxY = std::max(maxY, lpc[j]);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Lpcc: {
        auto frames = frameSignal(audio->samples, m_config.frameSize, true, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            auto lpc = computeLPC(frames[i], 13);
            auto lpcc = lpcToLpcc(lpc, 13);
            for (int j = 0; j < lpcc.size(); ++j) {
                pts.append({float(i * hopSize + j), lpcc[j]});
                minY = std::min(minY, lpcc[j]); maxY = std::max(maxY, lpcc[j]);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
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
            pts[i] = {float(i * result.frameSize), result.confidence[i]};
            minY = std::min(minY, result.confidence[i]); maxY = std::max(maxY, result.confidence[i]);
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Spectrogram: {
        // Spectrogram — 2D heatmap, compute but use via dedicated UI. Store as flat chart points.
        auto frames = frameSignal(audio->samples, m_config.frameSize, true, true);
        int hopSize = m_config.frameSize / 2;
        QVector<ChartPoint> pts;
        float minY = 1e30f, maxY = -1e30f;
        for (int i = 0; i < frames.size(); ++i) {
            FftProcessor fft(m_config.frameSize);
            auto mags = fft.computeMagnitudeSpectrum(frames[i]);
            for (int j = 0; j < mags.size(); ++j) {
                float db = mags[j] > 0 ? 20.0f * std::log10(mags[j] + 1e-10f) : -120.0f;
                pts.append({float(i * hopSize + j), db});
                minY = std::min(minY, db); maxY = std::max(maxY, db);
            }
        }
        chart.points = std::move(pts); chart.minY = minY; chart.maxY = maxY;
        break;
    }
    case DataType::Audio:
        return; // Already in cache
    }
    chart.visible = true;
    m_cache.add(filePath, chart);
}
