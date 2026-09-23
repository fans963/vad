#include "CepstralVad.h"
#include "../dsp/DspUtils.h"
#include "../dsp/FftProcessor.h"

VadResult CepstralVad::process(const QVector<float>& samples, uint32_t,
                                int frameSize) const {
    const int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;
    result.analysisFrameSize = frameSize;
    if (samples.size() < frameSize || frameSize <= 0) return result;

    auto frames = frameSignalFast(samples, frameSize, true, true, 0.95f);
    int nFrames = frames.size();
    if (nFrames == 0) return result;

    QVector<bool> voiceFlags(nFrames, false);
    FftProcessor fftCep(frameSize);

    for (int f = 0; f < nFrames; ++f) {
        const float* frameData = frames.frame(f);
        float energy = 0.0f;
        for (int j = 0; j < frameSize; ++j) {
            const float sample = frameData[j] * 32768.0f;
            energy += sample * sample;
        }
        float energyDB = 10.0f * std::log10(energy / frameSize + 1e-12f);
        if (energyDB < energyThresholdDB) continue;

        auto cep = fftCep.computeRealCepstrum(frameData, frameSize);

        QVector<float> vals;
        vals.reserve(cep.size() > 33 ? cep.size() - 33 : 0);
        for (int i = 33; i < cep.size(); ++i)
            vals.append(cep[i]);
        if (vals.isEmpty()) continue;

        std::sort(vals.begin(), vals.end());
        float median = vals[vals.size() / 2];

        float maxPeak = 0.0f;
        int maxIdx = 33;
        for (int i = 33; i < cep.size(); ++i) {
            if (cep[i] > maxPeak) { maxPeak = cep[i]; maxIdx = i; }
        }

        float secondPeak = 0.0f;
        for (int i = 33; i < cep.size(); ++i) {
            if (std::abs(i - maxIdx) > 25 && cep[i] > secondPeak)
                secondPeak = cep[i];
        }

        float p1 = maxPeak - median;
        float p2 = secondPeak - median;
        if (p2 < 1e-6f) p2 = 1e-6f;

        float divide = p1 / p2;
        voiceFlags[f] = (divide >= divideThreshold);
    }

    result.confidence.resize(nFrames);
    for (int i = 0; i < nFrames; ++i)
        result.confidence[i] = voiceFlags[i] ? 1.0f : 0.0f;

    QVector<float> smoothed = result.confidence;
    int runStart = -1;
    for (int i = 0; i <= nFrames; ++i) {
        bool voice = (i < nFrames && voiceFlags[i]);
        if (voice && runStart < 0) runStart = i;
        else if (!voice && runStart >= 0) {
            int runLen = i - runStart;
            if (runLen < minVoiceFrames) {
                for (int j = runStart; j < i; ++j)
                    smoothed[j] = 0.0f;
            }
            runStart = -1;
        }
    }
    result.confidence = smoothed;
    return result;
}
