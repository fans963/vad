#pragma once

#include "../core/Types.h"

// ── Energy-based VAD ────────────────────────────────────────────────────────
// Uses short-time energy threshold with hangover frames.
class EnergyVad {
public:
    float threshold = 0.05f;
    int hangoverFrames = 8;

    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;

    QString name() const { return QStringLiteral("energy"); }
    QString displayName() const { return QStringLiteral("Energy-based VAD"); }

    QVector<VadParamDef> parameters() const {
        return {
            VadParamDef::mkFloat("threshold", QStringLiteral("Energy Threshold"),
                                  threshold, 0.001f, 1.0f),
            VadParamDef::mkInt("hangover_frames", QStringLiteral("Hangover Frames"),
                                hangoverFrames, 0, 50),
        };
    }

    void setParameter(const QString& key, float value) {
        if (key == QLatin1String("threshold"))
            threshold = std::clamp(value, 0.001f, 1.0f);
        else if (key == QLatin1String("hangover_frames"))
            hangoverFrames = std::clamp((int)value, 0, 50);
    }
};
