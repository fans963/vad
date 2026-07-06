#pragma once

#include <QVector>
#include <cmath>

// ── Window types ────────────────────────────────────────────────────────────
enum class WindowType { Rectangular, Hamming, Hanning, Blackman };

// ── Frame signal ────────────────────────────────────────────────────────────
QVector<QVector<float>> frameSignal(const QVector<float>& samples,
                                     int frameSize,
                                     bool preEmphasis = false,
                                     bool hammingWindow = false,
                                     float preEmphasisAlpha = 0.95f);

// ── Sign function ───────────────────────────────────────────────────────────
inline float sgn(float x) { return x >= 0.0f ? 1.0f : -1.0f; }

// ── Window coefficients ─────────────────────────────────────────────────────
inline QVector<float> windowCoef(int size, WindowType type = WindowType::Hamming) {
    QVector<float> w(size);
    for (int i = 0; i < size; ++i) {
        double r = (double)i / (size - 1);
        switch (type) {
        case WindowType::Rectangular: w[i] = 1.0f; break;
        case WindowType::Hanning:     w[i] = 0.50f - 0.50f * std::cos(2.0 * M_PI * r); break;
        case WindowType::Blackman:    w[i] = 0.42f - 0.50f * std::cos(2.0 * M_PI * r)
                                            + 0.08f * std::cos(4.0 * M_PI * r); break;
        default:                      w[i] = 0.54f - 0.46f * std::cos(2.0 * M_PI * r); break;
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

// ── Short-time energy (dB scale) ────────────────────────────────────────────
inline float frameEnergyDB(const float* frame, int frameSize) {
    float sum = 0.0f;
    for (int i = 0; i < frameSize; ++i) {
        float s = frame[i] * 32768.0f;
        sum += s * s;
    }
    float mean = sum / frameSize;
    return mean > 0.0f ? 10.0f * std::log10(mean) : -100.0f;
}

// ── Short-time zero-crossing rate ───────────────────────────────────────────
inline float frameZCR(const float* frame, int frameSize) {
    int crossings = 0;
    for (int j = 0; j < frameSize - 1; ++j)
        if (sgn(frame[j]) != sgn(frame[j + 1])) crossings++;
    return float(crossings) / frameSize;
}
