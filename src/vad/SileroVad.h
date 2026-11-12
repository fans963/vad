#pragma once

#include "../core/Types.h"
#include <memory>

namespace ov {
class CompiledModel;
}

class SileroVad {
public:
    float threshold = 0.50f;
    float negThresholdOffset = 0.15f;
    int minSpeechMs = 250;
    int minSilenceMs = 100;
    int speechPadMs = 30;

    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;

    QString name() const { return QStringLiteral("silero"); }
    QString displayName() const { return QStringLiteral("Silero VAD (OpenVINO)"); }

    QVector<VadParamDef> parameters() const;
    void setParameter(const QString& key, float value);

    static QString resolveModelPath();
    static bool isModelAvailable();

private:
    static std::shared_ptr<ov::CompiledModel> getCompiledModel();
};
