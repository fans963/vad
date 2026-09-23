#pragma once

#include <QObject>
#include <QTimer>
#include <memory>
#include <limits>
#include <atomic>

#include "Types.h"

struct AudioPlayerDevice;

// ── Multi-platform audio player using miniaudio ─────────────────────────────
class AudioPlayer : public QObject {
    Q_OBJECT
public:
    explicit AudioPlayer(QObject* parent = nullptr);
    ~AudioPlayer();

    void load(const AudioData& audio);
    void play(uint64_t startSample = 0,
              uint64_t endSample = std::numeric_limits<uint64_t>::max());
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

private:
    void emitState();
    void initDevice();
    void uninitDevice();
    static void miniaudioDataCallback(void* pDevice, void* pOutput, const void* pInput, unsigned int frameCount);

    AudioData m_audio;
    std::unique_ptr<AudioPlayerDevice> m_device;
    QTimer* m_timer = nullptr;

    std::atomic<bool> m_playing{false};
    std::atomic<bool> m_finished{false};
    std::atomic<double> m_currentSample{0.0};
    std::atomic<uint64_t> m_endSample{0};
    std::atomic<float> m_speed{1.0f};
    uint64_t m_startSample = 0;
};
