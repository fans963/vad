#pragma once

#include <QVector>
#include <cstdint>

// ── Pitch detection results ─────────────────────────────────────────────────
struct PitchResult {
    QVector<float> pitch; // Hz per frame, 0 = unvoiced
    QVector<float> confidence; // per frame confidence
};

// ── ACF method ──────────────────────────────────────────────────────────────
PitchResult detectPitchACF(const QVector<float>& samples, int frameSize,
                            uint32_t sampleRate);

// ── AMDF method ─────────────────────────────────────────────────────────────
PitchResult detectPitchAMDF(const QVector<float>& samples, int frameSize,
                             uint32_t sampleRate);

// ── Cepstral method ─────────────────────────────────────────────────────────
PitchResult detectPitchCepstral(const QVector<float>& samples, int frameSize,
                                 uint32_t sampleRate);

// ── Helpers ─────────────────────────────────────────────────────────────────
float pitchFromIdx(int idx, uint32_t sampleRate);
