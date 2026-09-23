#include "ZcrVad.h"
#include "../dsp/DspUtils.h"
#include "../core/Parallel.h"

#include <algorithm>

VadResult ZcrVad::process(const QVector<float>& samples, uint32_t,
                           int frameSize) const {
    const int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;
    result.analysisFrameSize = frameSize;
    result.energyLabel = QStringLiteral("平均帧能量");
    result.zcrLabel = QStringLiteral("过零率");
    result.highEnergyThreshold = energyFloor;
    result.zcrThreshold = zcrThreshold;
    if (samples.size() < frameSize || frameSize <= 0) return result;

    auto frames = frameSignalFast(samples, frameSize, true, true);
    int nFrames = frames.size();
    if (nFrames <= 0) return result;

    result.confidence.resize(nFrames);
    result.energy.resize(nFrames);
    result.zcr.resize(nFrames);

    parallelFor(nFrames, [&](int i) {
        const float* frame = frames.frame(i);
        float energy = 0.0f;
        for (int j = 0; j < frameSize; ++j)
            energy += frame[j] * frame[j];
        energy /= frameSize;
        result.energy[i] = energy;

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
        result.zcr[i] = zcrVal;
        result.confidence[i] = 1.0f - std::min(zcrVal / zcrThreshold, 1.0f);
    }, 256);

    return result;
}
