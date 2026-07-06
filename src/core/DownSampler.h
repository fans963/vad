#pragma once

#include "Types.h"

// ── MinMax downsampling ─────────────────────────────────────────────────────
// Reduces chart to ~targetPoints buckets, preserving min/max per bucket for
// visual fidelity. Uses parallelFor for bucket computation.
CachedChart minMaxDownsample(const CachedChart& chart, int targetPoints);

// ── Equal-step downsampling ─────────────────────────────────────────────────
// Simple every-Nth-point reduction.
CachedChart equalStepDownsample(const CachedChart& chart, int targetPoints);
