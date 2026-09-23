#include "FftProcessor.h"
#include <fftw3.h>
#include <mutex>
#include <unordered_map>
#include <cmath>
#include <algorithm>

struct PlanCache {
    std::mutex mutex;
    std::unordered_map<int, fftw_plan> r2cPlans;
    std::unordered_map<int, fftw_plan> c2rPlans;

    ~PlanCache() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& [n, p] : r2cPlans) {
            if (p) fftw_destroy_plan(p);
        }
        for (auto& [n, p] : c2rPlans) {
            if (p) fftw_destroy_plan(p);
        }
        r2cPlans.clear();
        c2rPlans.clear();
    }

    fftw_plan getR2C(int n) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = r2cPlans.find(n);
        if (it != r2cPlans.end()) return it->second;

        double* dummy_in = fftw_alloc_real(n);
        fftw_complex* dummy_out = fftw_alloc_complex(n / 2 + 1);
        fftw_plan plan = fftw_plan_dft_r2c_1d(n, dummy_in, dummy_out, FFTW_ESTIMATE);
        fftw_free(dummy_out);
        fftw_free(dummy_in);
        r2cPlans[n] = plan;
        return plan;
    }

    fftw_plan getC2R(int n) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = c2rPlans.find(n);
        if (it != c2rPlans.end()) return it->second;

        fftw_complex* dummy_in = fftw_alloc_complex(n / 2 + 1);
        double* dummy_out = fftw_alloc_real(n);
        fftw_plan plan = fftw_plan_dft_c2r_1d(n, dummy_in, dummy_out, FFTW_ESTIMATE);
        fftw_free(dummy_out);
        fftw_free(dummy_in);
        c2rPlans[n] = plan;
        return plan;
    }
};

static PlanCache gPlanCache;

struct FftProcessor::Impl {
    int N = 0;
    double* in = nullptr;
    fftw_complex* out = nullptr;
    fftw_plan plan = nullptr;

    int invN = 0;
    fftw_complex* invIn = nullptr;
    double* invOut = nullptr;
    fftw_plan invPlan = nullptr;

    explicit Impl(int n) { init(n); }

    ~Impl() {
        if (in) fftw_free(in);
        if (out) fftw_free(out);
        if (invIn) fftw_free(invIn);
        if (invOut) fftw_free(invOut);
    }

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    void init(int n) {
        if (N == n && in && out && plan) return;
        if (in) fftw_free(in);
        if (out) fftw_free(out);
        N = n;
        in = fftw_alloc_real(N);
        out = fftw_alloc_complex(N / 2 + 1);
        plan = gPlanCache.getR2C(N);
    }

    void initInverse(int n) {
        if (invN == n && invIn && invOut && invPlan) return;
        if (invIn) fftw_free(invIn);
        if (invOut) fftw_free(invOut);
        invN = n;
        invIn = fftw_alloc_complex(invN / 2 + 1);
        invOut = fftw_alloc_real(invN);
        invPlan = gPlanCache.getC2R(invN);
    }

    QVector<float> run(const float* timeData, int size) {
        if (N != size) init(size);
        for (int i = 0; i < N; ++i)
            in[i] = (i < size) ? static_cast<double>(timeData[i]) : 0.0;
        fftw_execute_dft_r2c(plan, in, out);

        const int numBins = N / 2 + 1;
        QVector<float> mags(numBins);
        const double invN = 1.0 / N;
        for (int i = 0; i < numBins; ++i) {
            double re = out[i][0], im = out[i][1];
            double mag = std::sqrt(re * re + im * im) * invN;
            if (i > 0 && i < N / 2) mag *= 2.0;
            mags[i] = static_cast<float>(mag);
        }
        return mags;
    }
};

FftProcessor::FftProcessor(int fftSize) : d(std::make_unique<Impl>(fftSize)) {}
FftProcessor::~FftProcessor() = default;

QVector<float> FftProcessor::computeMagnitudeSpectrum(const QVector<float>& timeData) {
    return computeMagnitudeSpectrum(timeData.constData(), timeData.size());
}

QVector<float> FftProcessor::computeMagnitudeSpectrum(const float* timeData, int size) {
    if (!timeData || size <= 0) return {};
    return d->run(timeData, size);
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

QVector<float> FftProcessor::computeRealCepstrum(const QVector<float>& timeData) {
    return computeRealCepstrum(timeData.constData(), timeData.size());
}

QVector<float> FftProcessor::computeRealCepstrum(const float* timeData, int size) {
    if (!timeData || size <= 0) return {};
    d->init(size);
    d->initInverse(size);

    auto magnitude = d->run(timeData, size);
    const int numBins = size / 2 + 1;
    for (int i = 0; i < numBins; ++i) {
        d->invIn[i][0] = std::log(std::max<double>(magnitude[i], 1e-10));
        d->invIn[i][1] = 0.0;
    }

    fftw_execute_dft_c2r(d->invPlan, d->invIn, d->invOut);

    QVector<float> result(size);
    const double invSize = 1.0 / size;
    for (int i = 0; i < size; ++i)
        result[i] = static_cast<float>(d->invOut[i] * invSize);
    return result;
}

QVector<float> FftProcessor::computeMelSpectrum(const QVector<float>& magnitudes,
                                                  int sampleRate, int numFilters) {
    return computeMelSpectrum(magnitudes.constData(), magnitudes.size(), sampleRate, numFilters);
}

QVector<float> FftProcessor::computeMelSpectrum(const float* magnitudes, int size,
                                                  int sampleRate, int numFilters) {
    if (!magnitudes || size <= 0) return {};

    float maxFreq = sampleRate / 2.0f;
    float minMel = freqToMel(0.0f);
    float maxMel = freqToMel(maxFreq);
    float melStep = (maxMel - minMel) / (numFilters + 1);
    int N = (size - 1) * 2;

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
            if (k < 0 || k >= size) continue;
            float w = (k < binC) ? float(k - binL) / (binC - binL + 1e-5f)
                                 : float(binR - k) / (binR - binC + 1e-5f);
            filterBanks[i] += w * magnitudes[k] * magnitudes[k];
        }
    }
    return filterBanks;
}

QVector<float> FftProcessor::computeMFCC(const QVector<float>& timeData,
                                           int sampleRate, int numFilters, int numCoeffs) {
    return computeMFCC(timeData.constData(), timeData.size(), sampleRate, numFilters, numCoeffs);
}

QVector<float> FftProcessor::computeMFCC(const float* timeData, int size,
                                           int sampleRate, int numFilters, int numCoeffs) {
    auto mags = computeMagnitudeSpectrum(timeData, size);
    auto melSpec = computeMelSpectrum(mags.constData(), mags.size(), sampleRate, numFilters);

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
