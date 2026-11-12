#pragma once

#include <QVector>
#include <cmath>
#include <algorithm>
#include <numbers>
#include <span>

// ── Window types ────────────────────────────────────────────────────────────
enum class WindowType { Rectangular, Hamming, Hanning, Blackman };

// ── Contiguous framed signal representation (Zero-fragmentation) ─────────────
struct FramedSignal {
    int numFrames = 0;
    int frameSize = 0;
    int hopSize = 0;
    QVector<float> data; // Contiguous memory: numFrames * frameSize

    [[nodiscard]] constexpr bool isEmpty() const noexcept { return numFrames == 0; }
    [[nodiscard]] constexpr int size() const noexcept { return numFrames; }
    [[nodiscard]] constexpr const float* frame(int i) const noexcept { return data.constData() + i * frameSize; }
    [[nodiscard]] constexpr float* frame(int i) noexcept { return data.data() + i * frameSize; }
    [[nodiscard]] std::span<const float> frameSpan(int i) const noexcept {
        return {frame(i), static_cast<std::size_t>(frameSize)};
    }

    QVector<float> getFrameVector(int i) const {
        QVector<float> v(frameSize);
        std::copy_n(frame(i), frameSize, v.data());
        return v;
    }
};

FramedSignal frameSignalFast(const QVector<float>& samples,
                             int frameSize,
                             bool preEmphasis = false,
                             bool hammingWindow = false,
                             float preEmphasisAlpha = 0.95f);

// ── Legacy frameSignal (backward compatibility) ─────────────────────────────
QVector<QVector<float>> frameSignal(const QVector<float>& samples,
                                     int frameSize,
                                     bool preEmphasis = false,
                                     bool hammingWindow = false,
                                     float preEmphasisAlpha = 0.95f);

// ── Sign function ───────────────────────────────────────────────────────────
[[nodiscard]] constexpr float sgn(float x) noexcept { return x >= 0.0f ? 1.0f : -1.0f; }

// ── Window coefficients ─────────────────────────────────────────────────────
inline QVector<float> windowCoef(int size, WindowType type = WindowType::Hamming) {
    QVector<float> w(size);
    for (int i = 0; i < size; ++i) {
        double r = static_cast<double>(i) / (size - 1);
        switch (type) {
        case WindowType::Rectangular: w[i] = 1.0f; break;
        case WindowType::Hanning:     w[i] = 0.50f - 0.50f * std::cos(2.0 * std::numbers::pi * r); break;
        case WindowType::Blackman:    w[i] = 0.42f - 0.50f * std::cos(2.0 * std::numbers::pi * r)
                                            + 0.08f * std::cos(4.0 * std::numbers::pi * r); break;
        default:                      w[i] = 0.54f - 0.46f * std::cos(2.0 * std::numbers::pi * r); break;
        }
    }
    return w;
}

// ── Pre-emphasis ────────────────────────────────────────────────────────────
inline QVector<float> preEmphasize(const QVector<float>& samples, float alpha = 0.95f) {
    if (samples.size() < 2) return samples;
    QVector<float> out(samples.size());
    out[0] = samples[0];
    for (int i = 1; i < samples.size(); ++i)
        out[i] = samples[i] - alpha * samples[i - 1];
    return out;
}

// ── Short-time average amplitude ────────────────────────────────────────────
inline float frameAvgAmplitude(const float* frame, int frameSize) {
    float sum = 0.0f;
    for (int i = 0; i < frameSize; ++i)
        sum += std::fabs(frame[i]);
    return sum / frameSize;
}

// ── Short-time energy (linear scale) ────────────────────────────────────────
inline float frameEnergyLinear(const float* frame, int frameSize) {
    float sum = 0.0f;
    for (int i = 0; i < frameSize; ++i) {
        float s = frame[i] * 32768.0f;
        sum += s * s;
    }
    return sum;
}

// ── Short-time zero-crossing rate ───────────────────────────────────────────
inline float frameZCR(const float* frame, int frameSize) {
    int crossings = 0;
    for (int j = 0; j < frameSize - 1; ++j)
        if (sgn(frame[j]) != sgn(frame[j + 1])) crossings++;
    return float(crossings) / frameSize;
}
