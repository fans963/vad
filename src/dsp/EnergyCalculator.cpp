#include "EnergyCalculator.h"
#include "DspUtils.h"
#include "../core/DownSampler.h"
#include "../core/Parallel.h"

#include <limits>

float frameEnergy(const float* frame, int frameSize) {
    float sum = 0.0f;
    for (int i = 0; i < frameSize; ++i) {
        float s = frame[i] * 32768.0f;
        sum += s * s;
    }
    float mean = sum / frameSize;
    return mean > 0.0f ? 10.0f * std::log10(mean) : -100.0f;
}

CachedChart computeEnergy(const QVector<float>& samples, int frameSize, int downSampleTarget) {
    auto frames = frameSignal(samples, frameSize, false, false);
    int hopSize = frameSize / 2;
    int nFrames = frames.size();

    if (nFrames == 0) {
        CachedChart chart;
        chart.dataType = DataType::Energy;
        return chart;
    }

    // Parallel energy computation using Qt thread pool
    QVector<float> energies(nFrames);

    parallelFor(nFrames, [&](int i) {
        energies[i] = frameEnergy(frames[i].constData(), frameSize);
    });

    // Build result points
    QVector<ChartPoint> points(nFrames);
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < nFrames; ++i) {
        points[i] = {float(i * hopSize), energies[i]};
        minY = std::min(minY, energies[i]);
        maxY = std::max(maxY, energies[i]);
    }

    CachedChart chart;
    chart.dataType = DataType::Energy;
    chart.points = std::move(points);
    chart.minY = minY;
    chart.maxY = maxY;
    chart.visible = true;

    return minMaxDownsample(chart, downSampleTarget);
}
