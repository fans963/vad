#include "EnergyCalculator.h"
#include "DspUtils.h"
#include "../core/Parallel.h"

#include <limits>

float frameEnergy(const float* frame, int frameSize) {
    return frameEnergyLinear(frame, frameSize);
}

CachedChart computeEnergy(const QVector<float>& samples, int frameSize) {
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
        const float frameCenter = float(i * hopSize) + 0.5f * frameSize;
        points[i] = {frameCenter, energies[i]};
        minY = std::min(minY, energies[i]);
        maxY = std::max(maxY, energies[i]);
    }

    CachedChart chart;
    chart.dataType = DataType::Energy;
    chart.points = std::move(points);
    chart.minY = minY;
    chart.maxY = maxY;
    chart.visible = true;

    return chart;
}
