#include "dsp/FeatureExtractor.h"
#include "dsp/EnergyCalculator.h"
#include "dsp/PitchDetector.h"
#include "dsp/ZeroCrossingRate.h"
#include "vad/ReferenceVad.h"

#include <QCoreApplication>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
bool check(bool condition, const char* message) {
    if (!condition) std::cerr << "FAILED: " << message << '\n';
    return condition;
}
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    constexpr int sampleRate = 16000;
    constexpr int frameSize = 256;
    QVector<float> signal(sampleRate);
    for (int i = sampleRate / 4; i < sampleRate * 3 / 4; ++i)
        signal[i] = 0.7f * std::sin(2.0 * M_PI * 200.0 * i / sampleRate);

    bool ok = true;

    const float energyFrame[] = {0.25f, -0.25f, 0.5f, -0.5f};
    const float doubledEnergyFrame[] = {0.5f, -0.5f, 1.0f, -1.0f};
    const float energy = frameEnergy(energyFrame, 4);
    const float doubledEnergy = frameEnergy(doubledEnergyFrame, 4);
    ok &= check(energy > 0.0f, "short-time energy must stay on a linear scale");
    ok &= check(std::abs(doubledEnergy - 4.0f * energy) <= energy * 1e-5f,
                "doubling amplitude must quadruple linear short-time energy");

    const QVector<float> framedSignal = {
        0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f
    };
    const auto energyChart = computeEnergy(framedSignal, 4);
    const auto zcrChart = computeZCR(framedSignal, 4);
    ok &= check(energyChart.points.size() == 3 &&
                energyChart.points[0].x == 2.0f &&
                energyChart.points[1].x == 4.0f &&
                energyChart.points[2].x == 6.0f,
                "energy points must be located at frame centers");
    ok &= check(zcrChart.points.size() == 3 &&
                zcrChart.points[0].x == 2.0f &&
                zcrChart.points[1].x == 4.0f &&
                zcrChart.points[2].x == 6.0f,
                "ZCR points must be located at frame centers");

    const auto spectrum = FeatureExtractor::spectrum(
        signal, frameSize, DataType::Spectrum, sampleRate);
    const auto cepstrum = FeatureExtractor::spectrum(
        signal, frameSize, DataType::Cepstrum, sampleRate);
    const auto mel = FeatureExtractor::spectrum(
        signal, frameSize, DataType::MelSpectrum, sampleRate);
    const auto mfcc = FeatureExtractor::coefficients(
        signal, frameSize, DataType::Mfcc, sampleRate);
    const auto image = FeatureExtractor::spectrogram(signal, frameSize, sampleRate);

    ok &= check(!spectrum.points.isEmpty(), "spectrum must contain points");
    ok &= check(!cepstrum.points.isEmpty(), "real cepstrum must contain points");
    ok &= check(!mel.points.isEmpty() && !mfcc.points.isEmpty(),
                "Mel/MFCC must contain points");
    ok &= check(image.heatColumns > 0 && image.heatRows == frameSize / 2 + 1,
                "spectrogram dimensions must match STFT");

    ReferenceVad detector;
    const auto vad = detector.process(signal, sampleRate, frameSize);
    ok &= check(std::any_of(vad.confidence.begin(), vad.confidence.end(),
                            [](float value) { return value > 0.5f; }),
                "reference VAD must find the synthetic voiced segment");

    const auto pitch = detectPitchACF(signal, frameSize, sampleRate);
    int voicedFrames = 0;
    for (float hz : pitch.pitch)
        if (hz > 170.0f && hz < 230.0f) ++voicedFrames;
    ok &= check(voicedFrames > 0, "ACF detector must find a 200 Hz pitch");
    return ok ? 0 : 1;
}
