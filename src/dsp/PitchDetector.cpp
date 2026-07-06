#include "PitchDetector.h"
#include "DspUtils.h"
#include "../core/Parallel.h"
#include "FftProcessor.h"
#include <limits>
#include <cmath>

float pitchFromIdx(int idx, uint32_t sampleRate) {
    if (idx <= 0) return 0.0f;
    return (float)sampleRate / idx;
}

// ── ACF pitch ───────────────────────────────────────────────────────────────
PitchResult detectPitchACF(const QVector<float>& samples, int frameSize,
                            uint32_t sampleRate) {
    PitchResult r;

    auto frames = frameSignal(samples, frameSize, true, true);
    int nFrames = frames.size();
    r.pitch.resize(nFrames);
    r.confidence.resize(nFrames);

    parallelFor(nFrames, [&](int f) {
        const auto& frame = frames[f];
        int N = frame.size();
        int maxLag = N / 2;

        // Compute ACF
        QVector<float> acf(maxLag + 1, 0.0f);
        for (int k = 0; k <= maxLag; ++k) {
            float sum = 0.0f;
            for (int n = 0; n < N - k; ++n)
                sum += frame[n] * frame[n + k];
            acf[k] = sum;
        }

        // Center clipping for better pitch detection
        float clip = 0.70f * acf[0];
        QVector<float> clipped(N, 0.0f);
        for (int n = 0; n < N; ++n)
            clipped[n] = (std::fabs(frame[n]) > clip) ? frame[n] : 0.0f;

        // Re-compute ACF on clipped signal
        QVector<float> acf2(maxLag + 1, 0.0f);
        for (int k = 0; k <= maxLag; ++k) {
            float sum = 0.0f;
            for (int n = 0; n < N - k; ++n)
                sum += clipped[n] * clipped[n + k];
            acf2[k] = sum;
        }

        // Find peak in pitch range (50Hz-500Hz → index range)
        int minIdx = sampleRate / 500;
        int maxIdx = sampleRate / 50;
        minIdx = std::max(1, minIdx);
        maxIdx = std::min(maxLag, maxIdx);

        float maxVal = 0.0f;
        int bestIdx = 0;
        for (int k = minIdx; k <= maxIdx; ++k) {
            if (acf2[k] > maxVal) {
                maxVal = acf2[k];
                bestIdx = k;
            }
        }

        // Energy check for voiced
        float energy = 0.0f;
        for (int n = 0; n < N; ++n) energy += frame[n] * frame[n];
        energy = 10.0f * std::log10(energy / N + 1e-12f);

        if (energy > 30.0f && acf2[1] / std::max(acf2[0], 1e-10f) > 0.5f && bestIdx > 0) {
            r.pitch[f] = pitchFromIdx(bestIdx, sampleRate);
            r.confidence[f] = maxVal / std::max(acf2[0], 1e-10f);
        } else {
            r.pitch[f] = 0.0f;
            r.confidence[f] = 0.0f;
        }
    });

    return r;
}

// ── AMDF pitch ──────────────────────────────────────────────────────────────
PitchResult detectPitchAMDF(const QVector<float>& samples, int frameSize,
                             uint32_t sampleRate) {
    PitchResult r;

    auto frames = frameSignal(samples, frameSize, false, true);
    int nFrames = frames.size();
    r.pitch.resize(nFrames);
    r.confidence.resize(nFrames);

    parallelFor(nFrames, [&](int f) {
        const auto& frame = frames[f];
        int N = frame.size();
        int maxLag = N / 2;

        QVector<float> amdf(maxLag + 1, 0.0f);
        for (int k = 0; k <= maxLag; ++k) {
            float sum = 0.0f;
            int count = 0;
            for (int n = 0; n < N - k; ++n) {
                sum += std::fabs(frame[n] - frame[n + k]);
                count++;
            }
            amdf[k] = count > 0 ? sum / count : 0.0f;
        }

        // Find minimum in pitch range, skipping first few indices
        int minIdx = sampleRate / 500;
        int maxIdx = sampleRate / 50;
        minIdx = std::max(30, minIdx);
        maxIdx = std::min(maxLag, maxIdx);

        float minVal = std::numeric_limits<float>::max();
        int bestIdx = 0;
        for (int k = minIdx; k <= maxIdx; ++k) {
            if (amdf[k] < minVal) {
                minVal = amdf[k];
                bestIdx = k;
            }
        }

        if (bestIdx > 0) {
            r.pitch[f] = pitchFromIdx(bestIdx, sampleRate);
            r.confidence[f] = 1.0f - std::min(minVal / std::max(amdf[0], 1e-10f), 1.0f);
        } else {
            r.pitch[f] = 0.0f;
            r.confidence[f] = 0.0f;
        }
    });

    return r;
}

// ── Cepstral pitch ──────────────────────────────────────────────────────────
PitchResult detectPitchCepstral(const QVector<float>& samples, int frameSize,
                                 uint32_t sampleRate) {
    PitchResult r;
    auto frames = frameSignal(samples, frameSize, false, true);
    int nFrames = frames.size();
    r.pitch.resize(nFrames);
    r.confidence.resize(nFrames);

    for (int f = 0; f < nFrames; ++f) {
        QVector<float> logMag = FftProcessor::computeLogAmplitudeSpectrum(frames[f]);
        FftProcessor fftCep(logMag.size());
        auto cep = fftCep.computeMagnitudeSpectrum(logMag);

        int maxLag = cep.size() - 1;
        int minIdx = sampleRate / 500;
        int maxIdx = sampleRate / 50;
        minIdx = std::max(32, minIdx);
        maxIdx = std::min(maxLag, maxIdx);

        float maxVal = 0.0f;
        int bestIdx = 0;
        for (int k = minIdx; k <= maxIdx; ++k) {
            if (cep[k] > maxVal) { maxVal = cep[k]; bestIdx = k; }
        }
        if (bestIdx > 32) {
            r.pitch[f] = pitchFromIdx(bestIdx, sampleRate);
            r.confidence[f] = maxVal / std::max(cep[0], 1e-10f);
        } else {
            r.pitch[f] = 0.0f;
            r.confidence[f] = 0.0f;
        }
    }
    return r;
}
