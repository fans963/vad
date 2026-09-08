#include "ReferenceVad.h"

#include "../dsp/DspUtils.h"

#include <algorithm>
#include <cmath>

void ReferenceVad::setParameter(const QString& key, float value) {
    if (key == QLatin1String("low_energy_offset"))
        lowEnergyOffsetDb = std::clamp(value, -40.0f, -1.0f);
    else if (key == QLatin1String("high_energy_offset"))
        highEnergyOffsetDb = std::clamp(value, -30.0f, -1.0f);
    else if (key == QLatin1String("zcr_ratio"))
        zcrRatio = std::clamp(value, 0.01f, 1.0f);
    else if (key == QLatin1String("min_voice_frames"))
        minVoiceFrames = std::clamp(int(value), 1, 30);
}

VadResult ReferenceVad::process(const QVector<float>& samples, uint32_t,
                                int frameSize) const {
    VadResult result;
    result.frameSize = frameSize / 2;
    const auto frames = frameSignal(samples, frameSize, false, false);
    if (frames.isEmpty()) return result;

    QVector<float> energy(frames.size());
    QVector<float> zcr(frames.size());
    float maxEnergy = -120.0f;
    float maxZcr = 0.0f;
    for (int i = 0; i < frames.size(); ++i) {
        double sum = 0.0;
        int crossings = 0;
        for (int j = 0; j < frameSize; ++j) {
            const double sample = double(frames[i][j]) * 32768.0;
            sum += sample * sample;
            if (j + 1 < frameSize && sgn(frames[i][j]) != sgn(frames[i][j + 1]))
                ++crossings;
        }
        energy[i] = float(10.0 * std::log10(sum / frameSize + 1e-12));
        zcr[i] = float(crossings) / frameSize;
        maxEnergy = std::max(maxEnergy, energy[i]);
        maxZcr = std::max(maxZcr, zcr[i]);
    }

    const float high = maxEnergy + highEnergyOffsetDb;
    const float low = maxEnergy + lowEnergyOffsetDb;
    const float zcrThreshold = maxZcr * zcrRatio;

    QVector<QPair<int, int>> segments;
    int begin = -1;
    for (int i = 0; i <= energy.size(); ++i) {
        const bool above = i < energy.size() && energy[i] > high;
        if (above && begin < 0) begin = i;
        if (!above && begin >= 0) {
            if (i - begin >= minVoiceFrames)
                segments.append({begin, i - 1});
            begin = -1;
        }
    }

    for (auto& segment : segments) {
        while (segment.first > 0 && energy[segment.first - 1] > low)
            --segment.first;
        while (segment.second + 1 < energy.size() && energy[segment.second + 1] > low)
            ++segment.second;
        while (segment.first > 0 && zcr[segment.first - 1] >= zcrThreshold)
            --segment.first;
        while (segment.second + 1 < zcr.size() && zcr[segment.second + 1] >= zcrThreshold)
            ++segment.second;
    }

    // Merge overlaps introduced by threshold expansion.
    QVector<QPair<int, int>> merged;
    for (const auto& segment : segments) {
        if (!merged.isEmpty() && segment.first <= merged.last().second + 1)
            merged.last().second = std::max(merged.last().second, segment.second);
        else
            merged.append(segment);
    }

    result.confidence.fill(0.0f, frames.size());
    for (const auto& segment : merged)
        for (int i = segment.first; i <= segment.second; ++i)
            result.confidence[i] = 1.0f;
    return result;
}
