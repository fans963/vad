#pragma once

#include "Types.h"
#include "AudioDecoder.h"
#include "AudioStorage.h"
#include "ChartCache.h"
#include "AudioPlayer.h"
#include "DownSampler.h"
#include "../vad/VadEngine.h"

#include <QObject>
#include <memory>

// ── Audio processor engine (central orchestrator) ───────────────────────────
// Manages audio loading, decoding, chart computation, caching, VAD, and playback.
// Emits ChartEvent signals for the UI to consume.
class AudioEngine : public QObject {
    Q_OBJECT
public:
    explicit AudioEngine(QObject* parent = nullptr);

    // ── Configuration ─────────────────────────────────────────────────────
    void setConfig(const EngineConfig& config);
    void setDownSamplePointsNum(int n);
    void setIndexRange(float start, float end);
    EngineConfig config() const { return m_config; }

    // ── Audio file management ──────────────────────────────────────────────
    // Add an audio file (decode + store + create waveform chart)
    Q_INVOKABLE void addFile(const QString& filePath, const QByteArray& fileData,
                              const QString& format);

    // Compute a derived chart for a loaded file
    Q_INVOKABLE void addChart(const QString& filePath, DataType dataType);
    Q_INVOKABLE void removeChart(const QString& filePath, DataType dataType);
    Q_INVOKABLE void setChartVisibility(const QString& filePath, DataType dataType,
                                         bool visible);
    Q_INVOKABLE void removeAudio(const QString& filePath);

    // Audio info
    AudioInfo getAudioInfo(const QString& filePath) const;
    QStringList loadedFiles() const;

    // ── VAD ────────────────────────────────────────────────────────────────
    QStringList listVadAlgorithms() const;
    QString currentVadName() const;
    Q_INVOKABLE void setVadAlgorithm(const QString& name);
    QVector<VadParamDef> vadParams() const;
    Q_INVOKABLE void setVadParam(const QString& key, float value);

    // ── Audio playback ─────────────────────────────────────────────────────
    Q_INVOKABLE void playAudio(const QString& filePath, double startFraction = 0.0);
    Q_INVOKABLE void resumeAudio();
    Q_INVOKABLE void pauseAudio();
    Q_INVOKABLE void stopAudio();
    Q_INVOKABLE void seekAudio(double fraction);
    Q_INVOKABLE void setPlaybackSpeed(float multiplier);

signals:
    // Chart events for the UI
    void chartEvent(const ChartEvent& event);

private:
    void syncToUi();
    void recomputeRanges();
    void recomputeChartIntoCache(const QString& filePath, DataType dt);
    void emitRangeSignals();
    bool isFullRange() const;

    EngineConfig m_config;
    AudioDecoder m_decoder;
    AudioStorage m_storage;
    ChartCache m_cache;
    AudioPlayer m_player;
    VadEngine m_vad;

    // View state
    float m_indexRangeStart = 0.0f;
    float m_indexRangeEnd = std::numeric_limits<float>::max();
    float m_maxIndex = 10000.0f;
    float m_yMin = -0.5f;
    float m_yMax = 0.5f;
    int m_downSamplePointsNum = 600;
};
