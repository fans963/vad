#include "CepstralVad.h"
#include "../dsp/DspUtils.h"
#include "../dsp/FftProcessor.h"

VadResult CepstralVad::process(const QVector<float>& samples, uint32_t /*sr*/,
                                int frameSize) const {
    int hopSize = frameSize / 2;
    VadResult result;
    result.frameSize = hopSize;
    if (samples.size() < frameSize) return result;

    auto frames = frameSignal(samples, frameSize, true, true, 0.95f);
    int nFrames = frames.size();
    if (nFrames == 0) return result;

    // Per-frame voice detection using cepstral peak ratio
    QVector<bool> voiceFlags(nFrames, false);

    for (int f = 0; f < nFrames; ++f) {
        // Energy check
        float energy = 0.0f;
        for (int j = 0; j < frames[f].size(); ++j) {
            const float sample = frames[f][j] * 32768.0f;
            energy += sample * sample;
        }
        float energyDB = 10.0f * std::log10(energy / frameSize + 1e-12f);
        if (energyDB < energyThresholdDB) continue;

        // Cepstrum
        FftProcessor fftCep(frameSize);
        auto cep = fftCep.computeRealCepstrum(frames[f]);

        // Find median of cepstral values (ignoring first 33 points for DC)
        QVector<float> vals;
        for (int i = 33; i < cep.size(); ++i)
            vals.append(cep[i]);
        if (vals.isEmpty()) continue;

        std::sort(vals.begin(), vals.end());
        float median = vals[vals.size() / 2];

        // Find max peak and its cluster
        float maxPeak = 0.0f;
        int maxIdx = 33;
        for (int i = 33; i < cep.size(); ++i) {
            if (cep[i] > maxPeak) { maxPeak = cep[i]; maxIdx = i; }
        }

        // Find second peak outside the main peak cluster (within ±25 of maxIdx)
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

    // Group voice frames, filter short segments
    result.confidence.resize(nFrames);
    for (int i = 0; i < nFrames; ++i)
        result.confidence[i] = voiceFlags[i] ? 1.0f : 0.0f;

    // Smooth: remove isolated detections shorter than minVoiceFrames
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
