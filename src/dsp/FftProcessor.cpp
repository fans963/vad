#include "FftProcessor.h"
#include <fftw3.h>
#include <mutex>

// Plan creation is NOT thread-safe in FFTW3 — serialize globally
static std::mutex gFftwPlanMutex;

struct FftProcessor::Impl {
    int N;
    double* in = nullptr;
    fftw_complex* out = nullptr;
    fftw_plan plan = nullptr;

    Impl(int n) : N(n) {
        in = fftw_alloc_real(N);
        out = fftw_alloc_complex(N / 2 + 1);
        {
            std::lock_guard<std::mutex> lock(gFftwPlanMutex);
            plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
        }
    }
    ~Impl() {
        if (plan) {
            std::lock_guard<std::mutex> lock(gFftwPlanMutex);
            fftw_destroy_plan(plan);
        }
        if (in) fftw_free(in);
        if (out) fftw_free(out);
    }
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    QVector<float> run(const QVector<float>& timeData) {
        for (int i = 0; i < N; ++i)
            in[i] = (i < timeData.size()) ? timeData[i] : 0.0;
        fftw_execute(plan);

        QVector<float> mags(N / 2 + 1);
        for (int i = 0; i <= N / 2; ++i) {
            double re = out[i][0], im = out[i][1];
            double mag = std::sqrt(re * re + im * im) / N;
            if (i > 0 && i < N / 2) mag *= 2.0;
            mags[i] = float(mag);
        }
        return mags;
    }
};

FftProcessor::FftProcessor(int fftSize) : d(std::make_unique<Impl>(fftSize)) {}
FftProcessor::~FftProcessor() = default;

QVector<float> FftProcessor::computeMagnitudeSpectrum(const QVector<float>& timeData) {
    d->N = timeData.size();
    // Recreate plan if size changed
    d.reset(new Impl(timeData.size()));
    return d->run(timeData);
}

QVector<float> FftProcessor::computeSpectrumOfSpectrum(const QVector<float>& magnitudes) {
    return computeMagnitudeSpectrum(magnitudes);
}

QVector<float> FftProcessor::computeLogAmplitudeSpectrum(const QVector<float>& magnitudes) {
    QVector<float> logMags(magnitudes.size());
    for (int i = 0; i < magnitudes.size(); ++i)
        logMags[i] = std::log(magnitudes[i] + 1e-10f);
    return logMags;
}

QVector<float> FftProcessor::computeCepstrum(const QVector<float>& logMagnitudes) {
    return computeMagnitudeSpectrum(logMagnitudes);
}

QVector<float> FftProcessor::computeMelSpectrum(const QVector<float>& magnitudes,
                                                  int sampleRate, int numFilters) {
    if (magnitudes.empty()) return {};

    float maxFreq = sampleRate / 2.0f;
    float minMel = freqToMel(0.0f);
    float maxMel = freqToMel(maxFreq);
    float melStep = (maxMel - minMel) / (numFilters + 1);
    int N = (magnitudes.size() - 1) * 2;

    QVector<float> filterBanks(numFilters, 0.0f);
    for (int i = 0; i < numFilters; ++i) {
        float melCenter = minMel + (i + 1) * melStep;
        float fCenter = melToFreq(melCenter);
        float fLeft  = melToFreq(melCenter - melStep);
        float fRight = melToFreq(melCenter + melStep);
        int binL = std::floor(fLeft * N / sampleRate);
        int binC = std::floor(fCenter * N / sampleRate);
        int binR = std::floor(fRight * N / sampleRate);

        for (int k = binL; k <= binR; ++k) {
            if (k < 0 || k >= magnitudes.size()) continue;
            float w = (k < binC) ? float(k - binL) / (binC - binL + 1e-5f)
                                 : float(binR - k) / (binR - binC + 1e-5f);
            filterBanks[i] += w * magnitudes[k];
        }
    }
    return filterBanks;
}

QVector<float> FftProcessor::computeMFCC(const QVector<float>& timeData,
                                           int sampleRate, int numFilters, int numCoeffs) {
    auto mags = computeMagnitudeSpectrum(timeData);
    auto melSpec = computeMelSpectrum(mags, sampleRate, numFilters);

    QVector<float> logMelSpec(melSpec.size());
    for (int i = 0; i < melSpec.size(); ++i)
        logMelSpec[i] = std::log(melSpec[i] + 1e-10f);

    QVector<float> mfcc(numCoeffs, 0.0f);
    for (int i = 0; i < numCoeffs; ++i) {
        float sum = 0.0f;
        for (int j = 0; j < melSpec.size(); ++j)
            sum += logMelSpec[j] * std::cos(M_PI * i / melSpec.size() * (j + 0.5f));
        mfcc[i] = sum;
    }
    return mfcc;
}
