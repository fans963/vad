#pragma once

#include <QVector>
#include "../core/Types.h"

// ── Energy calculator ───────────────────────────────────────────────────────
// Computes per-frame energy in dB scale using parallel execution.
// Result points x = frame index * hopSize, y = energy in dB.
CachedChart computeEnergy(const QVector<float>& samples, int frameSize,
                          int downSampleTarget = 600);

// ── Energy for a single frame ───────────────────────────────────────────────
float frameEnergy(const float* frame, int frameSize);
