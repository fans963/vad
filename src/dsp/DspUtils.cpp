#include "DspUtils.h"

FramedSignal frameSignalFast(const QVector<float>& samples,
                             int frameSize,
                             bool preEmphasis,
                             bool hammingWindow,
                             float preEmphasisAlpha)
{
    FramedSignal result;
    if (samples.size() < frameSize || frameSize <= 0) return result;

    int hopSize = frameSize / 2;
    int numFrames = (samples.size() - frameSize) / hopSize + 1;
    if (numFrames <= 0) return result;

    result.numFrames = numFrames;
    result.frameSize = frameSize;
    result.hopSize = hopSize;
    result.data.resize(numFrames * frameSize);

    QVector<float> processed;
    const float* src = samples.constData();
    if (preEmphasis) {
        processed.resize(samples.size());
        processed[0] = samples[0];
        for (int i = 1; i < samples.size(); ++i)
            processed[i] = samples[i] - preEmphasisAlpha * samples[i - 1];
        src = processed.constData();
    }

    QVector<float> hamming;
    if (hammingWindow) {
        hamming.resize(frameSize);
        for (int i = 0; i < frameSize; ++i)
            hamming[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (frameSize - 1));
    }

    float* dst = result.data.data();
    for (int f = 0; f < numFrames; ++f) {
        const float* frameSrc = src + f * hopSize;
        float* frameDst = dst + f * frameSize;
        if (hammingWindow) {
            for (int i = 0; i < frameSize; ++i) {
                frameDst[i] = frameSrc[i] * hamming[i];
            }
        } else {
            std::copy_n(frameSrc, frameSize, frameDst);
        }
    }

    return result;
}

QVector<QVector<float>> frameSignal(const QVector<float>& samples,
                                     int frameSize,
                                     bool preEmphasis,
                                     bool hammingWindow,
                                     float preEmphasisAlpha)
{
    FramedSignal fast = frameSignalFast(samples, frameSize, preEmphasis, hammingWindow, preEmphasisAlpha);
    QVector<QVector<float>> frames;
    frames.reserve(fast.numFrames);
    for (int f = 0; f < fast.numFrames; ++f) {
        frames.append(fast.getFrameVector(f));
    }
    return frames;
}
