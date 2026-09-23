#include "AudioPlayer.h"

#include <miniaudio.h>
#include <algorithm>
#include <cstring>
#include <QDebug>

struct AudioPlayerDevice {
    ma_device device{};
    bool initialized = false;
};

void AudioPlayer::miniaudioDataCallback(void* pDevicePtr, void* pOutput, const void* /*pInput*/, unsigned int frameCount) {
    auto* pDevice = static_cast<ma_device*>(pDevicePtr);
    if (!pDevice) return;
    auto* player = static_cast<AudioPlayer*>(pDevice->pUserData);
    if (!player) return;

    float* out = static_cast<float*>(pOutput);
    const ma_uint32 channels = pDevice->playback.channels;

    if (!player->m_playing.load(std::memory_order_relaxed)) {
        std::memset(out, 0, frameCount * channels * sizeof(float));
        return;
    }

    const auto totalAudioSamples = static_cast<size_t>(player->m_audio.samples.size());
    const uint64_t end = player->m_endSample.load(std::memory_order_relaxed);
    const double speed = static_cast<double>(player->m_speed.load(std::memory_order_relaxed));
    const bool isStereo = (channels >= 2 && player->m_audio.info.channels >= 2 &&
                           static_cast<size_t>(player->m_audio.leftSamples.size()) == totalAudioSamples &&
                           static_cast<size_t>(player->m_audio.rightSamples.size()) == totalAudioSamples);

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        double curr = player->m_currentSample.load(std::memory_order_relaxed);
        if (curr >= end || curr >= totalAudioSamples) {
            std::memset(out + i * channels, 0, (frameCount - i) * channels * sizeof(float));
            player->m_playing.store(false, std::memory_order_relaxed);
            player->m_finished.store(true, std::memory_order_release);
            break;
        }

        const size_t i0 = static_cast<size_t>(curr);
        const size_t i1 = std::min(i0 + 1, totalAudioSamples > 0 ? totalAudioSamples - 1 : 0);
        const float frac = static_cast<float>(curr - i0);

        if (isStereo) {
            const float l = (1.0f - frac) * player->m_audio.leftSamples[i0] + frac * player->m_audio.leftSamples[i1];
            const float r = (1.0f - frac) * player->m_audio.rightSamples[i0] + frac * player->m_audio.rightSamples[i1];
            out[i * channels]     = l;
            out[i * channels + 1] = r;
            for (ma_uint32 ch = 2; ch < channels; ++ch) {
                out[i * channels + ch] = 0.0f;
            }
        } else {
            const float s = (1.0f - frac) * player->m_audio.samples[i0] + frac * player->m_audio.samples[i1];
            for (ma_uint32 ch = 0; ch < channels; ++ch) {
                out[i * channels + ch] = s;
            }
        }

        player->m_currentSample.store(curr + speed, std::memory_order_relaxed);
    }
}

AudioPlayer::AudioPlayer(QObject* parent)
    : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(30);
    connect(m_timer, &QTimer::timeout, this, &AudioPlayer::onTick);
}

AudioPlayer::~AudioPlayer() {
    stop();
    uninitDevice();
}

void AudioPlayer::load(const AudioData& audio) {
    stop();
    m_audio = audio;
    initDevice();
}

void AudioPlayer::play(uint64_t startSample, uint64_t endSample) {
    if (m_audio.samples.isEmpty()) return;

    stop();

    if (!m_device || !m_device->initialized) {
        initDevice();
    }
    if (!m_device || !m_device->initialized) {
        qWarning() << "[AudioPlayer] Failed to initialize audio playback device";
        return;
    }

    m_startSample = std::min(startSample, static_cast<uint64_t>(m_audio.samples.size()));
    const uint64_t clampedEnd = std::clamp(endSample, m_startSample, static_cast<uint64_t>(m_audio.samples.size()));
    m_endSample.store(clampedEnd);
    m_currentSample.store(static_cast<double>(m_startSample));
    m_finished.store(false);
    m_playing.store(true);

    if (ma_device_start(&m_device->device) != MA_SUCCESS) {
        m_playing.store(false);
        qWarning() << "[AudioPlayer] ma_device_start failed";
        return;
    }

    m_timer->start();
    emitState();
}

void AudioPlayer::resume() {
    if (!m_playing.load() && isLoaded() && m_currentSample.load() < m_endSample.load()) {
        if (!m_device || !m_device->initialized) {
            initDevice();
        }
        if (m_device && m_device->initialized) {
            m_finished.store(false);
            m_playing.store(true);
            if (ma_device_start(&m_device->device) != MA_SUCCESS) {
                m_playing.store(false);
                return;
            }
            m_timer->start();
            emitState();
        }
    }
}

void AudioPlayer::pause() {
    if (m_playing.load()) {
        m_playing.store(false);
        m_timer->stop();
        if (m_device && m_device->initialized) {
            ma_device_stop(&m_device->device);
        }
        emitState();
    }
}

void AudioPlayer::stop() {
    m_timer->stop();
    m_playing.store(false);
    if (m_device && m_device->initialized) {
        ma_device_stop(&m_device->device);
    }
    m_finished.store(false);
    m_startSample = 0;
    m_endSample.store(0);
    m_currentSample.store(0.0);
}

void AudioPlayer::seek(uint64_t sample) {
    const bool wasPlaying = m_playing.load();
    const uint64_t end = m_endSample.load();
    stop();
    const uint64_t clamped = std::min(sample, static_cast<uint64_t>(m_audio.samples.size()));
    m_currentSample.store(static_cast<double>(clamped));
    if (wasPlaying) {
        play(clamped, end > clamped ? end : static_cast<uint64_t>(m_audio.samples.size()));
    } else {
        emitState();
    }
}

void AudioPlayer::setSpeed(float multiplier) {
    m_speed.store(std::clamp(multiplier, 0.25f, 4.0f));
}

bool AudioPlayer::isPlaying() const {
    return m_playing.load(std::memory_order_relaxed);
}

bool AudioPlayer::isLoaded() const {
    return !m_audio.samples.isEmpty();
}

std::optional<uint64_t> AudioPlayer::totalSamples() const {
    if (m_audio.samples.isEmpty()) return std::nullopt;
    return static_cast<uint64_t>(m_audio.samples.size());
}

float AudioPlayer::positionSecs() const {
    if (m_audio.info.sampleRate == 0) return 0.0f;
    const double curr = m_currentSample.load(std::memory_order_relaxed);
    return static_cast<float>(curr / m_audio.info.sampleRate);
}

float AudioPlayer::durationSecs() const {
    return m_audio.info.durationSecs;
}

void AudioPlayer::onTick() {
    if (m_finished.exchange(false, std::memory_order_acq_rel) || !m_playing.load(std::memory_order_relaxed)) {
        m_playing.store(false);
        m_timer->stop();
        if (m_device && m_device->initialized) {
            ma_device_stop(&m_device->device);
        }
        emitState();
        emit playbackFinished();
        return;
    }
    emitState();
}

void AudioPlayer::emitState() {
    PlaybackState ps;
    ps.isPlaying = isPlaying();
    ps.position = positionSecs();
    ps.duration = durationSecs();
    const float chartPos = ps.position * m_audio.info.sampleRate;
    emit playbackStateChanged(ps, chartPos);
}

void AudioPlayer::initDevice() {
    uninitDevice();
    if (m_audio.info.sampleRate == 0 || m_audio.samples.isEmpty()) return;

    m_device = std::make_unique<AudioPlayerDevice>();
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = (m_audio.info.channels >= 2) ? 2 : 1;
    config.sampleRate = static_cast<ma_uint32>(m_audio.info.sampleRate);
    config.dataCallback = reinterpret_cast<ma_device_data_proc>(miniaudioDataCallback);
    config.pUserData = this;

    ma_result res = ma_device_init(nullptr, &config, &m_device->device);
    if (res != MA_SUCCESS) {
        qWarning() << "[AudioPlayer] ma_device_init failed with error:" << res;
        m_device.reset();
        return;
    }
    m_device->initialized = true;
}

void AudioPlayer::uninitDevice() {
    if (m_device) {
        if (m_device->initialized) {
            ma_device_stop(&m_device->device);
            ma_device_uninit(&m_device->device);
            m_device->initialized = false;
        }
        m_device.reset();
    }
}
