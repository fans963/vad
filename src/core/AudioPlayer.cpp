#include "AudioPlayer.h"
#include <algorithm>

AudioPlayer::AudioPlayer(QObject* parent) : QObject(parent) {
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect(m_timer, &QTimer::timeout, this, &AudioPlayer::onTick);
}

AudioPlayer::~AudioPlayer() { stop(); }

void AudioPlayer::load(const AudioData& audio) {
    stop();
    m_audio = audio;

    m_format.setSampleRate(audio.info.sampleRate);
    m_format.setChannelCount(std::min<uint16_t>(audio.info.channels, 2));
    m_format.setSampleFormat(QAudioFormat::Float);
}

void AudioPlayer::play(uint64_t startSample) {
    if (m_audio.samples.isEmpty() || m_playing) return;

    m_startSample = std::min(startSample, (uint64_t)m_audio.samples.size());

    int channels = m_format.channelCount();
    size_t totalSamples = (size_t)m_audio.samples.size() - m_startSample;
    QByteArray pcmData(static_cast<int>(totalSamples * channels * sizeof(float)), Qt::Uninitialized);
    float* dst = reinterpret_cast<float*>(pcmData.data());

    for (size_t i = 0; i < totalSamples; ++i) {
        float s = m_audio.samples[m_startSample + i];
        for (int ch = 0; ch < channels; ++ch)
            *dst++ = s;
    }

    m_buffer = std::make_unique<QBuffer>();
    m_buffer->setData(pcmData);
    m_buffer->open(QIODevice::ReadOnly);

    m_sink = std::make_unique<QAudioSink>(m_format);
    connect(m_sink.get(), &QAudioSink::stateChanged,
            this, &AudioPlayer::onStateChanged);

    m_playing = true;
    m_sink->start(m_buffer.get());
    m_timer->start();
    emitState();
}

void AudioPlayer::resume() {
    if (m_sink && !m_playing) {
        m_sink->resume();
        m_playing = true;
        m_timer->start();
        emitState();
    }
}

void AudioPlayer::pause() {
    if (m_sink && m_playing) {
        m_sink->suspend();
        m_playing = false;
        m_timer->stop();
        emitState();
    }
}

void AudioPlayer::stop() {
    m_timer->stop();
    m_sink.reset();
    m_buffer.reset();
    m_playing = false;
    m_startSample = 0;
}

void AudioPlayer::seek(uint64_t) {
    // Re-create from new position
    bool wasPlaying = m_playing;
    stop();
    // Seeking not fully supported via QAudioSink — restart play at new position
    (void)wasPlaying;
}

void AudioPlayer::setSpeed(float m) {
    m_speed = std::clamp(m, 0.25f, 4.0f);
}

bool AudioPlayer::isPlaying() const { return m_playing; }
bool AudioPlayer::isLoaded() const { return !m_audio.samples.isEmpty(); }

std::optional<uint64_t> AudioPlayer::totalSamples() const {
    if (m_audio.samples.isEmpty()) return std::nullopt;
    return m_audio.samples.size();
}

float AudioPlayer::positionSecs() const {
    if (m_audio.info.sampleRate == 0) return 0.0f;
    uint64_t elapsed = 0;
    if (m_sink && m_playing) {
        // Estimate from bytes written vs bytes processed
        if (m_buffer)
            elapsed = m_sink->elapsedUSecs() / 1000 * m_audio.info.sampleRate / 1000;
    }
    uint64_t total = m_startSample + elapsed;
    if (m_audio.info.sampleRate > 0)
        return total / (float)m_audio.info.sampleRate;
    return 0.0f;
}

float AudioPlayer::durationSecs() const { return m_audio.info.durationSecs; }

void AudioPlayer::onTick() {
    if (!m_sink || !m_playing) return;
    emitState();
}

void AudioPlayer::onStateChanged(QAudio::State state) {
    if (state == QAudio::IdleState || state == QAudio::StoppedState) {
        m_playing = false;
        m_timer->stop();
        emitState();
        emit playbackFinished();
    }
}

void AudioPlayer::emitState() {
    PlaybackState ps;
    ps.isPlaying = m_playing;
    ps.position = positionSecs();
    ps.duration = durationSecs();
    float chartPos = ps.position; // Simplified
    emit playbackStateChanged(ps, chartPos);
}
