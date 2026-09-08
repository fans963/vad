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

    m_format.setSampleRate(std::max(1, int(audio.info.sampleRate * m_speed)));
    m_format.setChannelCount(std::min<uint16_t>(audio.info.channels, 2));
    m_format.setSampleFormat(QAudioFormat::Float);
}

void AudioPlayer::play(uint64_t startSample, uint64_t endSample) {
    if (m_audio.samples.isEmpty() || m_playing) return;

    m_startSample = std::min(startSample, (uint64_t)m_audio.samples.size());
    m_endSample = std::clamp(endSample, m_startSample, uint64_t(m_audio.samples.size()));

    int channels = m_format.channelCount();
    size_t totalSamples = m_endSample - m_startSample;
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
    m_endSample = 0;
}

void AudioPlayer::seek(uint64_t sample) {
    bool wasPlaying = m_playing;
    stop();
    m_startSample = std::min(sample, uint64_t(m_audio.samples.size()));
    if (wasPlaying)
        play(m_startSample);
    else
        emitState();
}

void AudioPlayer::setSpeed(float m) {
    const bool wasPlaying = m_playing;
    const uint64_t position = uint64_t(positionSecs() * m_audio.info.sampleRate);
    m_speed = std::clamp(m, 0.25f, 4.0f);
    m_format.setSampleRate(std::max(1, int(m_audio.info.sampleRate * m_speed)));
    if (wasPlaying) {
        stop();
        play(position);
    }
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
        if (m_buffer)
            elapsed = uint64_t(m_sink->processedUSecs() * m_audio.info.sampleRate
                               * m_speed / 1000000.0);
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
    const float chartPos = ps.position * m_audio.info.sampleRate;
    emit playbackStateChanged(ps, chartPos);
}
