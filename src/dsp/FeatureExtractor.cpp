#include "FeatureExtractor.h"

#include "DspUtils.h"
#include "FftProcessor.h"
#include "LpcProcessor.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {

void appendFrame(CachedChart& chart, const QVector<float>& values,
                 int frame, int hopSize) {
    const float base = float(frame * hopSize);
    const float spacing = values.isEmpty() ? 0.0f : float(hopSize) / values.size();
    for (int i = 0; i < values.size(); ++i) {
        const float value = std::isfinite(values[i]) ? values[i] : 0.0f;
        chart.points.append({base + i * spacing, value});
        chart.minY = std::min(chart.minY, value);
        chart.maxY = std::max(chart.maxY, value);
    }
}

CachedChart beginChart(DataType type) {
    CachedChart chart;
    chart.dataType = type;
    chart.visible = true;
    chart.minY = std::numeric_limits<float>::max();
    chart.maxY = std::numeric_limits<float>::lowest();
    return chart;
}

void finishChart(CachedChart& chart) {
    if (chart.points.isEmpty() && chart.heatValues.isEmpty()) {
        chart.minY = 0.0f;
        chart.maxY = 0.0f;
    }
}

} // namespace

namespace FeatureExtractor {

CachedChart spectrum(const QVector<float>& samples, int frameSize,
                     DataType type, uint32_t sampleRate) {
    auto chart = beginChart(type);
    const auto frames = frameSignalFast(samples, frameSize, true, true, 0.95f);
    const int hop = frameSize / 2;
    FftProcessor fft(frameSize);

    int pointsPerFrame = 0;
    switch (type) {
    case DataType::Spectrum:
    case DataType::PowerSpectrum:
    case DataType::LogSpectrum:
    case DataType::SpectrumFFT:
        pointsPerFrame = frameSize / 2 + 1;
        break;
    case DataType::Cepstrum:
        pointsPerFrame = frameSize;
        break;
    case DataType::MelSpectrum:
        pointsPerFrame = 24;
        break;
    default:
        break;
    }
    if (pointsPerFrame > 0) {
        chart.points.reserve(frames.size() * pointsPerFrame);
    }

    for (int frameIndex = 0; frameIndex < frames.size(); ++frameIndex) {
        const float* frameData = frames.frame(frameIndex);
        QVector<float> values;
        switch (type) {
        case DataType::Spectrum:
            values = fft.computeMagnitudeSpectrum(frameData, frameSize);
            break;
        case DataType::PowerSpectrum: {
            values = fft.computeMagnitudeSpectrum(frameData, frameSize);
            for (int i = 0; i < values.size(); ++i)
                values[i] = values[i] * values[i];
            break;
        }
        case DataType::LogSpectrum: {
            values = fft.computeMagnitudeSpectrum(frameData, frameSize);
            for (int i = 0; i < values.size(); ++i)
                values[i] = 10.0f * std::log10(values[i] * values[i] + 1e-20f);
            break;
        }
        case DataType::Cepstrum:
            values = fft.computeRealCepstrum(frameData, frameSize);
            break;
        case DataType::SpectrumFFT: {
            const auto magnitude = fft.computeMagnitudeSpectrum(frameData, frameSize);
            values = fft.computeSpectrumOfSpectrum(magnitude);
            break;
        }
        case DataType::MelSpectrum: {
            const auto magnitude = fft.computeMagnitudeSpectrum(frameData, frameSize);
            values = FftProcessor::computeMelSpectrum(magnitude, sampleRate, 24);
            break;
        }
        default:
            break;
        }
        appendFrame(chart, values, frameIndex, hop);
    }
    finishChart(chart);
    return chart;
}

CachedChart coefficients(const QVector<float>& samples, int frameSize,
                         DataType type, uint32_t sampleRate) {
    auto chart = beginChart(type);
    const auto frames = frameSignalFast(samples, frameSize, true, true, 0.95f);
    const int hop = frameSize / 2;
    FftProcessor fft(frameSize);
    const int pointsPerFrame = (type == DataType::Mfcc) ? 13 : 14;
    chart.points.reserve(frames.size() * pointsPerFrame);

    for (int frameIndex = 0; frameIndex < frames.size(); ++frameIndex) {
        const float* frameData = frames.frame(frameIndex);
        QVector<float> values;
        if (type == DataType::Mfcc) {
            values = fft.computeMFCC(frameData, frameSize, sampleRate, 24, 13);
        } else {
            const auto lpc = computeLPC(frameData, frameSize, 13);
            values = (type == DataType::Lpc) ? lpc : lpcToLpcc(lpc, 13);
        }
        appendFrame(chart, values, frameIndex, hop);
    }
    finishChart(chart);
    return chart;
}

CachedChart spectrogram(const QVector<float>& samples, int frameSize,
                        uint32_t sampleRate) {
    auto chart = beginChart(DataType::Spectrogram);
    const auto frames = frameSignalFast(samples, frameSize, true, true, 0.95f);
    const int bins = frameSize / 2 + 1;
    chart.heatColumns = frames.size();
    chart.heatRows = bins;
    chart.heatXMax = frames.isEmpty() ? 0.0f : float((frames.size() - 1) * (frameSize / 2));
    chart.heatYMax = sampleRate / 2.0f;
    chart.heatValues.resize(chart.heatColumns * bins);

    FftProcessor fft(frameSize);
    float minDb = std::numeric_limits<float>::max();
    float maxDb = std::numeric_limits<float>::lowest();
    for (int f = 0; f < frames.size(); ++f) {
        const auto mag = fft.computeMagnitudeSpectrum(frames.frame(f), frameSize);
        float* rowDst = chart.heatValues.data() + f * bins;
        for (int b = 0; b < bins; ++b) {
            const float db = 20.0f * std::log10(mag[b] + 1e-10f);
            rowDst[b] = db;
            minDb = std::min(minDb, db);
            maxDb = std::max(maxDb, db);
        }
    }
    chart.minY = minDb;
    chart.maxY = maxDb;
    finishChart(chart);
    return chart;
}

CachedChart reconstructLpc(const QVector<float>& samples, int frameSize) {
    auto chart = beginChart(DataType::LpcReconstructed);
    if (samples.size() < frameSize) {
        finishChart(chart);
        return chart;
    }
    const int hop = frameSize / 2;
    const auto frames = frameSignalFast(samples, frameSize, true, true, 0.95f);
    QVector<float> output(samples.size(), 0.0f);
    QVector<float> weight(samples.size(), 0.0f);
    for (int f = 0; f < frames.size(); ++f) {
        const auto lpc = computeLPC(frames.frame(f), frameSize, 13);
        const float* frameData = frames.frame(f);
        for (int n = 0; n < frameSize; ++n) {
            float prediction = 0.0f;
            for (int k = 1; k < lpc.size() && k <= n; ++k)
                prediction -= lpc[k] * frameData[n - k];
            const int index = f * hop + n;
            output[index] += prediction;
            weight[index] += 1.0f;
        }
    }
    chart.points.reserve(output.size());
    for (int i = 0; i < output.size(); ++i) {
        const float value = weight[i] > 0.0f ? output[i] / weight[i] : 0.0f;
        chart.points.append({float(i), value});
        chart.minY = std::min(chart.minY, value);
        chart.maxY = std::max(chart.maxY, value);
    }
    finishChart(chart);
    return chart;
}

} // namespace FeatureExtractor
