#include "DownSampler.h"
#include "Parallel.h"
#include <algorithm>

CachedChart minMaxDownsample(const CachedChart& chart, int targetPoints) {
    const auto& pts = chart.points;
    int n = pts.size();
    if (n <= targetPoints || n <= 2) {
        CachedChart result = chart;
        return result;
    }

    int buckets = std::max(2, targetPoints / 2);
    if (buckets >= n) buckets = n / 2;
    if (buckets < 2) buckets = 2;

    int bucketSize = n / buckets;
    if (bucketSize < 1) bucketSize = 1;

    // Each bucket produces 2 points (min, max)
    // We'll store them interleaved
    QVector<ChartPoint> result;
    result.reserve(buckets * 2 + 2);

    // Pre-allocate per-bucket min/max arrays
    QVector<int> minIdx(buckets, -1), maxIdx(buckets, -1);

    // Compute min/max per bucket in parallel
    parallelFor(buckets, [&](int b) {
        int bStart = b * bucketSize;
        int bEnd = std::min(bStart + bucketSize, n);
        if (bStart >= bEnd) return;

        int locMin = bStart, locMax = bStart;
        for (int i = bStart + 1; i < bEnd; ++i) {
            if (pts[i].y < pts[locMin].y) locMin = i;
            if (pts[i].y > pts[locMax].y) locMax = i;
        }
        minIdx[b] = locMin;
        maxIdx[b] = locMax;
    });

    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    // Always include first point
    result.append(pts[0]);
    minY = std::min(minY, pts[0].y);
    maxY = std::max(maxY, pts[0].y);

    // Interleave min and max per bucket, ensuring monotonic x
    for (int b = 0; b < buckets; ++b) {
        int mi = minIdx[b], mx = maxIdx[b];
        if (mi < 0 || mx < 0) continue;

        const auto& pMin = pts[mi];
        const auto& pMax = pts[mx];

        if (pMin.x <= pMax.x) {
            if (result.last().x < pMin.x) {
                result.append(pMin);
                minY = std::min(minY, pMin.y);
                maxY = std::max(maxY, pMin.y);
            }
            if (result.last().x < pMax.x) {
                result.append(pMax);
                minY = std::min(minY, pMax.y);
                maxY = std::max(maxY, pMax.y);
            }
        } else {
            if (result.last().x < pMax.x) {
                result.append(pMax);
                minY = std::min(minY, pMax.y);
                maxY = std::max(maxY, pMax.y);
            }
            if (result.last().x < pMin.x) {
                result.append(pMin);
                minY = std::min(minY, pMin.y);
                maxY = std::max(maxY, pMin.y);
            }
        }
    }

    // Always include last point
    if (result.last().x < pts[n - 1].x) {
        result.append(pts[n - 1]);
        minY = std::min(minY, pts[n - 1].y);
        maxY = std::max(maxY, pts[n - 1].y);
    }

    CachedChart out;
    out.dataType = chart.dataType;
    out.points = std::move(result);
    out.minY = (minY <= maxY) ? minY : chart.minY;
    out.maxY = (minY <= maxY) ? maxY : chart.maxY;
    out.visible = chart.visible;
    return out;
}

CachedChart equalStepDownsample(const CachedChart& chart, int targetPoints) {
    const auto& pts = chart.points;
    int n = pts.size();
    if (n <= targetPoints) return chart;

    int step = std::max(1, n / targetPoints);
    QVector<ChartPoint> result;
    result.reserve(n / step + 1);
    for (int i = 0; i < n; i += step)
        result.append(pts[i]);
    if (result.last().x < pts[n - 1].x)
        result.append(pts[n - 1]);

    CachedChart out = chart;
    out.points = std::move(result);
    return out;
}
