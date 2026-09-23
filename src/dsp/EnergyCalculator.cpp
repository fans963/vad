#include "EnergyCalculator.h"
#include "DspUtils.h"
#include "../core/Parallel.h"

#include <limits>

float frameEnergy(const float* frame, int frameSize) {
    return frameEnergyLinear(frame, frameSize);
}

CachedChart computeEnergy(const QVector<float>& samples, int frameSize) {
    CachedChart chart;
    chart.dataType = DataType::Energy;
    if (samples.size() < frameSize || frameSize <= 0) {
        return chart;
    }

    const int hopSize = frameSize / 2;
    const int nFrames = (samples.size() - frameSize) / hopSize + 1;
    if (nFrames <= 0) return chart;

    QVector<float> energies(nFrames);
    const float* src = samples.constData();

    parallelFor(nFrames, [&](int i) {
        energies[i] = frameEnergy(src + i * hopSize, frameSize);
    }, 512);

    QVector<ChartPoint> points(nFrames);
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < nFrames; ++i) {
        const float frameCenter = float(i * hopSize) + 0.5f * frameSize;
        points[i] = {frameCenter, energies[i]};
        minY = std::min(minY, energies[i]);
        maxY = std::max(maxY, energies[i]);
    }

    chart.points = std::move(points);
    chart.minY = minY;
    chart.maxY = maxY;
    chart.visible = true;

    return chart;
}
