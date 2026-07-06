#pragma once

#include <QString>
#include <QColor>
#include <QVector>
#include <cstdint>

// ── Chart point ─────────────────────────────────────────────────────────────
struct ChartPoint {
    float x = 0.0f;
    float y = 0.0f;
};

// ── Spectrogram heatmap point ───────────────────────────────────────────────
struct HeatPoint {
    float x = 0.0f;  // time (frame index)
    float y = 0.0f;  // frequency bin
    float z = 0.0f;  // magnitude (→ color)
};

// ── Data type enum ──────────────────────────────────────────────────────────
enum class DataType {
    Audio, Spectrum, Energy, ZeroCrossingRate, Vad,
    AvgAmplitude,       // 短时平均振幅
    SpectrumFFT,        // 频谱的FFT（二次频谱）
    AutoCorrelation,    // 修正自相关函数
    Lpc,                // LPC 线性预测系数
    Lpcc,               // LPC 倒谱系数
    PitchAcf,           // ACF 基音周期
    PitchAmdf,          // AMDF 基音周期
    PitchCep,           // 倒谱法基音周期
    CepstralVad,        // 倒谱端点检测 VAD
    Spectrogram,        // 语谱图
};

inline QString dataTypeName(DataType dt) {
    switch (dt) {
    case DataType::Audio:            return QStringLiteral("audio");
    case DataType::Spectrum:         return QStringLiteral("spectrum");
    case DataType::Energy:           return QStringLiteral("energy");
    case DataType::ZeroCrossingRate: return QStringLiteral("zcr");
    case DataType::Vad:              return QStringLiteral("vad");
    case DataType::AvgAmplitude:     return QStringLiteral("avgamp");
    case DataType::SpectrumFFT:      return QStringLiteral("specfft");
    case DataType::AutoCorrelation:  return QStringLiteral("acf");
    case DataType::Lpc:              return QStringLiteral("lpc");
    case DataType::Lpcc:             return QStringLiteral("lpcc");
    case DataType::PitchAcf:         return QStringLiteral("pitch_acf");
    case DataType::PitchAmdf:        return QStringLiteral("pitch_amdf");
    case DataType::PitchCep:         return QStringLiteral("pitch_cep");
    case DataType::CepstralVad:      return QStringLiteral("cepvad");
    case DataType::Spectrogram:      return QStringLiteral("spectrogram");
    }
    return {};
}

inline QString dataTypeDisplayName(DataType dt) {
    switch (dt) {
    case DataType::Audio:            return QStringLiteral("Waveform");
    case DataType::Spectrum:         return QStringLiteral("Spectrum");
    case DataType::Energy:           return QStringLiteral("Energy");
    case DataType::ZeroCrossingRate: return QStringLiteral("ZCR");
    case DataType::Vad:              return QStringLiteral("VAD");
    case DataType::AvgAmplitude:     return QStringLiteral("Avg Ampl");
    case DataType::SpectrumFFT:      return QStringLiteral("Spec FFT");
    case DataType::AutoCorrelation:  return QStringLiteral("AutoCorr");
    case DataType::Lpc:              return QStringLiteral("LPC");
    case DataType::Lpcc:             return QStringLiteral("LPCC");
    case DataType::PitchAcf:         return QStringLiteral("Pitch ACF");
    case DataType::PitchAmdf:        return QStringLiteral("Pitch AMDF");
    case DataType::PitchCep:         return QStringLiteral("Pitch Cep");
    case DataType::CepstralVad:      return QStringLiteral("Cep VAD");
    case DataType::Spectrogram:      return QStringLiteral("Spectrogram");
    }
    return {};
}

// ── Cached chart ────────────────────────────────────────────────────────────
struct CachedChart {
    DataType dataType = DataType::Audio;
    QVector<ChartPoint> points;
    float minY = 0.0f;
    float maxY = 0.0f;
    bool visible = true;

    CachedChart getRange(float start, float end) const;
};

// ── Audio info ──────────────────────────────────────────────────────────────
struct AudioInfo {
    QString filePath;
    QString format;
    uint32_t sampleRate = 0;
    uint16_t channels = 0;
    uint64_t sampleCount = 0;
    float durationSecs = 0.0f;
};

// ── Decoded audio data ──────────────────────────────────────────────────────
struct AudioData {
    QVector<float> samples;
    AudioInfo info;
};

// ── Engine configuration ────────────────────────────────────────────────────
struct EngineConfig {
    int frameSize = 256;
    int downSamplePointsNum = 600;
};

// ── VAD parameter definition ────────────────────────────────────────────────
enum class VadParamKind { Float, Int, Bool };

struct VadParamDef {
    QString key;
    QString label;
    VadParamKind kind = VadParamKind::Float;
    float value = 0.0f;
    float min = 0.0f;
    float max = 1.0f;
    float step = 0.01f;

    static VadParamDef mkFloat(const QString& k, const QString& l,
                                float v, float mn, float mx, float st = 0.001f) {
        return {k, l, VadParamKind::Float, v, mn, mx, st};
    }
    static VadParamDef mkInt(const QString& k, const QString& l,
                              int v, int mn, int mx) {
        return {k, l, VadParamKind::Int, float(v), float(mn), float(mx), 1.0f};
    }
    static VadParamDef mkBool(const QString& k, const QString& l, bool v) {
        return {k, l, VadParamKind::Bool, v ? 1.0f : 0.0f, 0.0f, 1.0f, 1.0f};
    }
};

// ── VAD result ──────────────────────────────────────────────────────────────
struct VadResult {
    QVector<float> confidence;
    uint32_t frameSize = 0;
};

// ── Playback state ──────────────────────────────────────────────────────────
struct PlaybackState {
    bool isPlaying = false;
    float position = 0.0f;
    float duration = 0.0f;
};

// ── Chart event ─────────────────────────────────────────────────────────────
struct ChartEvent {
    enum Type { AddChart, UpdateAllCharts, RemoveChart, RemoveAllCharts,
                UpdateMaxIndex, UpdateYRange, UpdatePlaybackState };
    Type type{UpdateAllCharts};
    QString key;
    DataType dataType{DataType::Audio};
    CachedChart chart;
    QVector<QPair<QString, CachedChart>> chartList;
    float maxIndex = 0.0f;
    float minY = 0.0f, maxY = 0.0f;
    PlaybackState playbackState;
    float chartPosition = 0.0f;
};

// ── Series key helpers ──────────────────────────────────────────────────────
inline QString makeSeriesKey(const QString& fp, DataType dt) {
    return fp + QStringLiteral("|") + dataTypeName(dt);
}
inline QPair<QString, DataType> parseSeriesKey(const QString& key) {
    int idx = key.lastIndexOf(QLatin1Char('|'));
    QString fp = key.left(idx);
    QString dtName = key.mid(idx + 1);
    DataType dt = DataType::Audio;
    if (dtName == QLatin1String("audio"))         dt = DataType::Audio;
    else if (dtName == QLatin1String("spectrum")) dt = DataType::Spectrum;
    else if (dtName == QLatin1String("energy"))   dt = DataType::Energy;
    else if (dtName == QLatin1String("zcr"))      dt = DataType::ZeroCrossingRate;
    else if (dtName == QLatin1String("vad"))      dt = DataType::Vad;
    else if (dtName == QLatin1String("avgamp"))   dt = DataType::AvgAmplitude;
    else if (dtName == QLatin1String("specfft"))  dt = DataType::SpectrumFFT;
    else if (dtName == QLatin1String("acf"))      dt = DataType::AutoCorrelation;
    else if (dtName == QLatin1String("lpc"))      dt = DataType::Lpc;
    else if (dtName == QLatin1String("lpcc"))     dt = DataType::Lpcc;
    else if (dtName == QLatin1String("pitch_acf"))  dt = DataType::PitchAcf;
    else if (dtName == QLatin1String("pitch_amdf")) dt = DataType::PitchAmdf;
    else if (dtName == QLatin1String("pitch_cep"))  dt = DataType::PitchCep;
    else if (dtName == QLatin1String("cepvad"))     dt = DataType::CepstralVad;
    else if (dtName == QLatin1String("spectrogram")) dt = DataType::Spectrogram;
    return {fp, dt};
}
