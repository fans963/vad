#include "DspUtils.h"

QVector<QVector<float>> frameSignal(const QVector<float>& samples,
                                     int frameSize,
                                     bool preEmphasis,
                                     bool hammingWindow,
                                     float preEmphasisAlpha)
{
    QVector<QVector<float>> frames;
    if (samples.size() < frameSize || frameSize <= 0) return frames;

    int hopSize = frameSize / 2;
    int numFrames = (samples.size() - frameSize) / hopSize + 1;
    if (numFrames <= 0) return frames;

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

    frames.reserve(numFrames);
    for (int f = 0; f < numFrames; ++f) {
        int start = f * hopSize;
        QVector<float> frame(frameSize);
        for (int i = 0; i < frameSize; ++i) {
            float v = src[start + i];
            if (hammingWindow) v *= hamming[i];
            frame[i] = v;
        }
        frames.append(std::move(frame));
    }
    return frames;
}
