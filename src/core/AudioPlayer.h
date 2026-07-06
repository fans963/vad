#pragma once

#include <QObject>
#include <QTimer>
#include <QAudioSink>
#include <QAudioFormat>
#include <QBuffer>
#include <memory>

#include "Types.h"

// ── Audio player using Qt6 QAudioSink ───────────────────────────────────────
class AudioPlayer : public QObject {
    Q_OBJECT
public:
    explicit AudioPlayer(QObject* parent = nullptr);
    ~AudioPlayer();

    void load(const AudioData& audio);
    void play(uint64_t startSample = 0);
    void resume();
    void pause();
    void stop();
    void seek(uint64_t samplePos);
    void setSpeed(float multiplier);
    bool isPlaying() const;
    bool isLoaded() const;
    std::optional<uint64_t> totalSamples() const;
    float positionSecs() const;
    float durationSecs() const;

signals:
    void playbackStateChanged(const PlaybackState& state, float chartPosition);
    void playbackFinished();

private slots:
    void onTick();
    void onStateChanged(QAudio::State state);

private:
    void emitState();

    AudioData m_audio;
    QAudioFormat m_format;
    std::unique_ptr<QAudioSink> m_sink;
    std::unique_ptr<QBuffer> m_buffer;
    QTimer* m_timer = nullptr;
    bool m_playing = false;
    uint64_t m_startSample = 0;
    float m_speed = 1.0f;
};
