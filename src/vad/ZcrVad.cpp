#include "ZcrVad.h"
#include "../dsp/DspUtils.h"
#include "../core/Parallel.h"

VadResult ZcrVad::process(const QVector<float>& samples, uint32_t /*sampleRate*/,
                           int frameSize) const {
    int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;
    if (samples.size() < frameSize) return result;

    auto frames = frameSignal(samples, frameSize, true, true);
    int nFrames = frames.size();
    result.confidence.resize(nFrames);

    // Parallel ZCR per frame (each frame is independent)
    parallelFor(nFrames, [&](int i) {
        const auto& frame = frames[i];
        float energy = 0.0f;
        for (int j = 0; j < frameSize; ++j)
            energy += frame[j] * frame[j];
        energy /= frameSize;

        if (energy < energyFloor) {
            result.confidence[i] = 0.0f;
            return;
        }
        int crossings = 0;
        for (int j = 0; j < frameSize - 1; ++j) {
            if (sgn(frame[j]) != sgn(frame[j + 1]))
                crossings++;
        }
        float zcrVal = float(crossings) / frameSize;
        result.confidence[i] = 1.0f - std::min(zcrVal / zcrThreshold, 1.0f);
    });

    return result;
}
