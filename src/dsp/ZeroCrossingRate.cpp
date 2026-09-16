#include "ZeroCrossingRate.h"
#include "DspUtils.h"
#include "../core/Parallel.h"

#include <limits>

CachedChart computeZCR(const QVector<float>& samples, int frameSize) {
    auto frames = frameSignal(samples, frameSize, true, true);
    int hopSize = frameSize / 2;
    int nFrames = frames.size();

    if (nFrames == 0) {
        CachedChart chart;
        chart.dataType = DataType::ZeroCrossingRate;
        return chart;
    }

    // Parallel ZCR computation using Qt thread pool
    QVector<float> zcrs(nFrames);

    parallelFor(nFrames, [&](int i) {
        zcrs[i] = frameZCR(frames[i].constData(), frameSize);
    });

    // Build result points
    QVector<ChartPoint> points(nFrames);
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < nFrames; ++i) {
        const float frameCenter = float(i * hopSize) + 0.5f * frameSize;
        points[i] = {frameCenter, zcrs[i]};
        minY = std::min(minY, zcrs[i]);
        maxY = std::max(maxY, zcrs[i]);
    }

    CachedChart chart;
    chart.dataType = DataType::ZeroCrossingRate;
    chart.points = std::move(points);
    chart.minY = minY;
    chart.maxY = maxY;
    chart.visible = true;

    return chart;
}
