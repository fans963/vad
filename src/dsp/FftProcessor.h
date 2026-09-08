#pragma once

#include <QVector>
#include <memory>

// Forward declarations for stdexec
namespace exec {
    class static_thread_pool;
}

class FftProcessor {
public:
    explicit FftProcessor(int fftSize);
    ~FftProcessor();

    // Compute magnitude spectrum (single frame)
    QVector<float> computeMagnitudeSpectrum(const QVector<float>& timeData);

    // Spectrum of spectrum (FFT of magnitude spectrum)
    QVector<float> computeSpectrumOfSpectrum(const QVector<float>& magnitudes);

    // Log amplitude spectrum
    static QVector<float> computeLogAmplitudeSpectrum(const QVector<float>& magnitudes);

    // Cepstrum (FFT of log amplitude spectrum)
    QVector<float> computeCepstrum(const QVector<float>& logMagnitudes);

    // Real cepstrum: IFFT(log(|FFT(x)|)).
    QVector<float> computeRealCepstrum(const QVector<float>& timeData);

    // Mel-scale spectrum
    static QVector<float> computeMelSpectrum(const QVector<float>& magnitudes,
                                              int sampleRate, int numFilters = 24);

    // MFCC
    QVector<float> computeMFCC(const QVector<float>& timeData,
                                int sampleRate, int numFilters = 24, int numCoeffs = 12);

    static float freqToMel(float freq) { return 2595.0f * std::log10(1.0f + freq / 700.0f); }
    static float melToFreq(float mel)  { return 700.0f * (std::pow(10.0f, mel / 2595.0f) - 1.0f); }

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
