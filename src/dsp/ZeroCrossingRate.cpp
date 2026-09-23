#include "ZeroCrossingRate.h"
#include "DspUtils.h"
#include "../core/Parallel.h"

#include <limits>

CachedChart computeZCR(const QVector<float>& samples, int frameSize) {
    CachedChart chart;
    chart.dataType = DataType::ZeroCrossingRate;

    auto frames = frameSignalFast(samples, frameSize, true, true);
    int hopSize = frameSize / 2;
    int nFrames = frames.size();

    if (nFrames == 0) {
        return chart;
    }

    QVector<float> zcrs(nFrames);

    parallelFor(nFrames, [&](int i) {
        zcrs[i] = frameZCR(frames.frame(i), frameSize);
    }, 512);

    QVector<ChartPoint> points(nFrames);
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    for (int i = 0; i < nFrames; ++i) {
        const float frameCenter = float(i * hopSize) + 0.5f * frameSize;
        points[i] = {frameCenter, zcrs[i]};
        minY = std::min(minY, zcrs[i]);
        maxY = std::max(maxY, zcrs[i]);
    }

    chart.points = std::move(points);
    chart.minY = minY;
    chart.maxY = maxY;
    chart.visible = true;

    return chart;
}
