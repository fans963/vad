#include "Types.h"
#include <algorithm>

CachedChart CachedChart::getRange(float start, float end) const {
    CachedChart result;
    result.dataType = dataType;
    result.visible = visible;
    result.minY = minY;
    result.maxY = maxY;
    result.heatValues = heatValues;
    result.heatColumns = heatColumns;
    result.heatRows = heatRows;
    result.heatXMax = heatXMax;
    result.heatYMax = heatYMax;

    if (points.isEmpty()) return result;

    // Binary search for start and end indices
    auto cmpLower = [](const ChartPoint& p, float v) { return p.x < v; };
    auto cmpUpper = [](float v, const ChartPoint& p) { return v < p.x; };
    int lo = std::lower_bound(points.begin(), points.end(), start, cmpLower) - points.begin();
    int hi = std::upper_bound(points.begin(), points.end(), end, cmpUpper) - points.begin();
    lo = std::max(0, lo - 1); // include one point before for visual continuity
    hi = std::min((int)points.size(), hi + 1); // include one after
    lo = std::max(0, lo);
    hi = std::min((int)points.size(), hi);

    result.points.reserve(hi - lo);
    for (int i = lo; i < hi; ++i)
        result.points.append(points[i]);

    if (result.points.isEmpty() && !points.isEmpty()) {
        // Fallback: always include at least first/last visible
        result.points.append(points.first());
        result.points.append(points.last());
    }

    return result;
}
