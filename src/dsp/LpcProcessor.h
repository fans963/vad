#pragma once

#include <QVector>
#include "../core/Types.h"

// ── LPC coefficients via Levinson-Durbin recursion ──────────────────────────
// Input: frame samples (windowed + pre-emphasized)
// Output: lpc[0..order] where lpc[0]=1.0
QVector<float> computeLPC(const QVector<float>& frame, int order = 13);

// ── LPCC from LPC coefficients ────────────────────────────────────────────
QVector<float> lpcToLpcc(const QVector<float>& lpc, int cepOrder = 13);

// ── Auto-correlation of a frame (for LPC input) ───────────────────────────
QVector<float> autoCorrelation(const QVector<float>& frame, int order = 13);
