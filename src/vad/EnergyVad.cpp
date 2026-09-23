#include "EnergyVad.h"
#include "../dsp/DspUtils.h"

VadResult EnergyVad::process(const QVector<float>& samples, uint32_t,
                              int frameSize) const {
    const int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;
    result.analysisFrameSize = frameSize;
    result.energyLabel = QStringLiteral("平均帧能量");

    if (samples.size() < frameSize || frameSize <= 0) return result;
    const int nFrames = (samples.size() - frameSize) / hopSize + 1;
    if (nFrames <= 0) return result;

    result.confidence.resize(nFrames);
    result.energy.resize(nFrames);
    result.highEnergyThreshold = threshold;

    const float* src = samples.constData();
    int hangover = 0;
    for (int i = 0; i < nFrames; ++i) {
        const float* frameData = src + i * hopSize;
        float energy = 0.0f;
        for (int j = 0; j < frameSize; ++j)
            energy += frameData[j] * frameData[j];
        energy /= frameSize;
        result.energy[i] = energy;

        bool voice = energy > threshold;
        if (voice)
            hangover = hangoverFrames;
        else if (hangover > 0)
            hangover--;

        result.confidence[i] = (hangover > 0 || voice) ? 1.0f : 0.0f;
    }
    return result;
}
