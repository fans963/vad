#pragma once

#include "../core/Types.h"

namespace FeatureExtractor {

CachedChart spectrum(const QVector<float>& samples, int frameSize,
                     DataType type, uint32_t sampleRate);
CachedChart coefficients(const QVector<float>& samples, int frameSize,
                         DataType type, uint32_t sampleRate);
CachedChart spectrogram(const QVector<float>& samples, int frameSize,
                        uint32_t sampleRate);
CachedChart reconstructLpc(const QVector<float>& samples, int frameSize);

} // namespace FeatureExtractor
