#pragma once

#include <QVector>
#include "../core/Types.h"

// ── Energy calculator ───────────────────────────────────────────────────────
// Computes per-frame linear energy (sum of squared PCM amplitudes) in parallel.
// Result points x = frame index * hopSize, y = linear short-time energy.
CachedChart computeEnergy(const QVector<float>& samples, int frameSize);

// ── Energy for a single frame ───────────────────────────────────────────────
float frameEnergy(const float* frame, int frameSize);
