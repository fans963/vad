#pragma once

#include <QVector>
#include "../core/Types.h"

// ── Zero-crossing rate calculator ───────────────────────────────────────────
// Computes per-frame ZCR using pre-emphasis and Hamming window.
// Uses parallel execution for frame processing.
CachedChart computeZCR(const QVector<float>& samples, int frameSize,
                        int downSampleTarget = 600);

// ── ZCR for a single frame ──────────────────────────────────────────────────
float frameZCR(const float* frame, int frameSize);
