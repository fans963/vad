#include "EnergyVad.h"
#include "../dsp/DspUtils.h"

VadResult EnergyVad::process(const QVector<float>& samples, uint32_t /*sampleRate*/,
                              int frameSize) const {
    int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;

    if (samples.size() < frameSize) return result;

    auto frames = frameSignal(samples, frameSize, false, false);
    int nFrames = frames.size();
    result.confidence.resize(nFrames);

    int hangover = 0;
    for (int i = 0; i < nFrames; ++i) {
        const auto& frame = frames[i];
        float energy = 0.0f;
        for (int j = 0; j < frameSize; ++j)
            energy += frame[j] * frame[j];
        energy /= frameSize;

        bool voice = energy > threshold;
        if (voice)
            hangover = hangoverFrames;
        else if (hangover > 0)
            hangover--;

        result.confidence[i] = (hangover > 0 || voice) ? 1.0f : 0.0f;
    }
    return result;
}
