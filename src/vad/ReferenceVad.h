#pragma once

#include "../core/Types.h"

// The two-threshold energy + ZCR endpoint detector used by the MyWave sample.
class ReferenceVad {
public:
    float lowEnergyOffsetDb = -20.0f;
    float highEnergyOffsetDb = -10.0f;
    float zcrRatio = 0.20f;
    int minVoiceFrames = 3;

    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;
    QString name() const { return QStringLiteral("reference"); }
    QVector<VadParamDef> parameters() const {
        return {
            VadParamDef::mkFloat("low_energy_offset", QStringLiteral("低能量门限 (dB)"),
                                 lowEnergyOffsetDb, -40.0f, -1.0f, 1.0f),
            VadParamDef::mkFloat("high_energy_offset", QStringLiteral("高能量门限 (dB)"),
                                 highEnergyOffsetDb, -30.0f, -1.0f, 1.0f),
            VadParamDef::mkFloat("zcr_ratio", QStringLiteral("过零率比例"),
                                 zcrRatio, 0.01f, 1.0f, 0.01f),
            VadParamDef::mkInt("min_voice_frames", QStringLiteral("最小语音帧数"),
                               minVoiceFrames, 1, 30),
        };
    }
    void setParameter(const QString& key, float value);
};
