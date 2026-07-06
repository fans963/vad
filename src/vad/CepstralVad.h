#pragma once

#include "../core/Types.h"

// ── Cepstral endpoint detection (teacher's original VAD method) ─────────────
// Uses cepstral peak energy cluster per frame with divide ratio threshold.
class CepstralVad {
public:
    float divideThreshold = 2.25f; // voiced if p1/p2 >= 2.25
    int minVoiceFrames = 3;
    float energyThresholdDB = 30.0f;

    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;

    QString name() const { return QStringLiteral("cepstral"); }
    QString displayName() const { return QStringLiteral("Cepstral VAD"); }

    QVector<VadParamDef> parameters() const {
        return {
            VadParamDef::mkFloat("divide_threshold",
                QStringLiteral("Divide Thr"), divideThreshold, 1.0f, 5.0f, 0.1f),
            VadParamDef::mkFloat("energy_threshold",
                QStringLiteral("Energy dB"), energyThresholdDB, 10.0f, 60.0f, 1.0f),
            VadParamDef::mkInt("min_voice_frames",
                QStringLiteral("Min Frames"), minVoiceFrames, 1, 10),
        };
    }

    void setParameter(const QString& key, float value) {
        if (key == QLatin1String("divide_threshold"))
            divideThreshold = std::clamp(value, 1.0f, 5.0f);
        else if (key == QLatin1String("energy_threshold"))
            energyThresholdDB = std::clamp(value, 10.0f, 60.0f);
        else if (key == QLatin1String("min_voice_frames"))
            minVoiceFrames = std::clamp((int)value, 1, 10);
    }
};
