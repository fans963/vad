#pragma once

#include "../core/Types.h"
#include "EnergyVad.h"
#include "ZcrVad.h"
#include "CepstralVad.h"
#include "ReferenceVad.h"
#include "SileroVad.h"
#include <variant>
#include <shared_mutex>
#include <mutex>

class VadEngine {
public:
    VadEngine();
    QStringList listAlgorithms() const;
    QString currentName() const;
    void setAlgorithm(const QString& name);
    QVector<VadParamDef> parameters() const;
    void setParameter(const QString& key, float value);
    VadResult process(const QVector<float>& samples, uint32_t sampleRate,
                      int frameSize) const;
private:
    using V = std::variant<ReferenceVad, EnergyVad, ZcrVad, CepstralVad, SileroVad>;
    V m_algo;
    mutable std::shared_mutex m_mutex;
};
