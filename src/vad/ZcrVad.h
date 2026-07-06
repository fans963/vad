#pragma once

#include "../core/Types.h"

// ── Zero-Crossing Rate based VAD ────────────────────────────────────────────
// Uses ZCR threshold with energy floor gating.
class ZcrVad {
public:
    float zcrThreshold = 0.3f;
    float energyFloor = 0.001f;

    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;

    QString name() const { return QStringLiteral("zcr"); }
    QString displayName() const { return QStringLiteral("ZCR-based VAD"); }

    QVector<VadParamDef> parameters() const {
        return {
            VadParamDef::mkFloat("zcr_threshold", QStringLiteral("ZCR Threshold"),
                                  zcrThreshold, 0.01f, 1.0f),
            VadParamDef::mkFloat("energy_floor", QStringLiteral("Energy Floor"),
                                  energyFloor, 0.0f, 0.1f),
        };
    }

    void setParameter(const QString& key, float value) {
        if (key == QLatin1String("zcr_threshold"))
            zcrThreshold = std::clamp(value, 0.01f, 1.0f);
        else if (key == QLatin1String("energy_floor"))
            energyFloor = std::clamp(value, 0.0f, 0.1f);
    }
};
