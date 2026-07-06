#include "VadEngine.h"

namespace {
    template<typename... Ts> struct OL : Ts... { using Ts::operator()...; };

    QString doName(const auto& v) { return v.name(); }
    QString doDisp(const auto& v) { return v.displayName(); }
    auto doParams(const auto& v) { return v.parameters(); }
    void doSetParam(auto& v, const QString& k, float val) { v.setParameter(k, val); }
    VadResult doProc(const auto& v, const QVector<float>& s, uint32_t sr, int fs) {
        return v.process(s, sr, fs);
    }
}

VadEngine::VadEngine() : m_algo(EnergyVad{}) {}

QStringList VadEngine::listAlgorithms() const {
    return {QStringLiteral("energy"), QStringLiteral("zcr"), QStringLiteral("cepstral")};
}

QString VadEngine::currentName() const {
    return std::visit([](auto& v) { return v.name(); }, m_algo);
}

void VadEngine::setAlgorithm(const QString& name) {
    if (name == QLatin1String("zcr")) m_algo = ZcrVad{};
    else if (name == QLatin1String("cepstral")) m_algo = CepstralVad{};
    else m_algo = EnergyVad{};
}

QVector<VadParamDef> VadEngine::parameters() const {
    return std::visit([](auto& v) { return v.parameters(); }, m_algo);
}

void VadEngine::setParameter(const QString& key, float value) {
    std::visit([&](auto& v) { v.setParameter(key, value); }, m_algo);
}

VadResult VadEngine::process(const QVector<float>& samples, uint32_t sampleRate,
                              int frameSize) const {
    return std::visit([&](auto& v) { return v.process(samples, sampleRate, frameSize); }, m_algo);
}
