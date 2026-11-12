#include "app/MainWindow.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QElapsedTimer>
#include <cmath>
#include <numbers>
#include <iostream>
#include <cstdint>

namespace {

bool check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << std::endl;
        return false;
    }
    return true;
}

// Generate simple 16-bit PCM WAV file
bool createWavFile(const QString& path, int sampleRate, int sampleCount, float frequency) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;

    const uint32_t dataSize = sampleCount * sizeof(int16_t);
    const uint32_t chunkSize = 36 + dataSize;
    const uint16_t numChannels = 1;
    const uint16_t bitsPerSample = 16;
    const uint32_t byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    const uint16_t blockAlign = numChannels * (bitsPerSample / 8);

    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVEfmt ", 8);
    const uint32_t subchunk1Size = 16;
    const uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);

    for (int i = 0; i < sampleCount; ++i) {
        const float s = std::sin(2.0 * std::numbers::pi * frequency * i / sampleRate);
        const int16_t val = static_cast<int16_t>(std::clamp(s * 32000.0f, -32768.0f, 32767.0f));
        file.write(reinterpret_cast<const char*>(&val), sizeof(int16_t));
    }
    file.close();
    return true;
}

void waitForEvents(int ms = 100) {
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < ms) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    }
}

} // namespace

int main(int argc, char** argv) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    const QString tempDir = QDir::tempPath();
    const QString wavA = tempDir + "/test_speech_a.wav";
    const QString wavB = tempDir + "/test_speech_b.wav";

    createWavFile(wavA, 16000, 16000, 440.0f); // 1 sec
    createWavFile(wavB, 16000, 8000, 880.0f);  // 0.5 sec

    bool ok = true;

    {
        MainWindow win;
        win.show();
        waitForEvents(50);

        // Load both files
        win.loadAudioFile(wavA);
        waitForEvents(50);
        win.loadAudioFile(wavB);
        waitForEvents(50);

        ok &= check(win.currentFilePath() == wavB, "Active file after loading B must be wavB");

        // 1. Switch to wavA
        win.setCurrentFile(wavA);
        waitForEvents(50);
        ok &= check(win.currentFilePath() == wavA, "Current file must switch to wavA");

        // Add Energy curve to wavA
        win.onMenuCurveToggled(DataType::Energy, true);
        waitForEvents(100);

        ok &= check(win.isMenuActionChecked(DataType::Energy),
                    "Energy curve menu action must be checked for wavA");

        // 2. Switch to wavB
        win.setCurrentFile(wavB);
        waitForEvents(50);
        ok &= check(win.currentFilePath() == wavB, "Current file must switch to wavB");

        // Crucial test: wavB has NOT added Energy curve, so Energy checkbox must be UNCHECKED!
        ok &= check(!win.isMenuActionChecked(DataType::Energy),
                    "Energy curve menu action must NOT be checked for wavB");

        // Add ZeroCrossingRate to wavB
        win.onMenuCurveToggled(DataType::ZeroCrossingRate, true);
        waitForEvents(100);
        ok &= check(win.isMenuActionChecked(DataType::ZeroCrossingRate),
                    "ZCR menu action must be checked for wavB");
        ok &= check(!win.isMenuActionChecked(DataType::Energy),
                    "Energy menu action must still remain unchecked for wavB");

        // 3. Switch back to wavA
        win.setCurrentFile(wavA);
        waitForEvents(50);
        ok &= check(win.isMenuActionChecked(DataType::Energy),
                    "Energy menu action must be checked when returning to wavA");
        ok &= check(!win.isMenuActionChecked(DataType::ZeroCrossingRate),
                    "ZCR menu action must NOT be checked for wavA");

        // 4. Test X-Axis constraint and zoom overlap prevention
        ChartWidget* cw = win.chartWidget();
        ok &= check(cw != nullptr, "ChartWidget must exist");
        ok &= check(cw->plot() != nullptr, "QCustomPlot inside ChartWidget must exist");

        // Horizontal-only interaction checks
        ok &= check(cw->plot()->axisRect()->rangeDrag() == Qt::Horizontal,
                    "Range drag must be constrained to Horizontal");
        ok &= check(cw->plot()->axisRect()->rangeZoom() == Qt::Horizontal,
                    "Range zoom must be constrained to Horizontal");
        ok &= check(cw->plot()->xAxis->numberFormat() == "f",
                    "X axis number format must be 'f'");
        ok &= check(cw->plot()->xAxis->numberPrecision() == 0,
                    "X axis number precision must be 0 (sample indices)");

        // Test zoom-in constraint: requested span of 10 samples must be clamped to at least 64
        cw->setXRange(1000.0, 1010.0);
        waitForEvents(50);
        const double span = cw->xRange().size();
        ok &= check(span >= 64.0, "X-axis range size must be constrained to at least 64 samples");

        // Test out-of-bounds drag constraint: negative coordinates must be clamped to 0
        cw->setXRange(-500.0, 500.0);
        waitForEvents(50);
        ok &= check(cw->xRange().lower >= 0.0, "X-axis lower bound must never be negative");

        // Test upper bound constraint
        const double maxIdx = cw->maxIndex();
        cw->setXRange(maxIdx + 1000.0, maxIdx + 2000.0);
        waitForEvents(50);
        ok &= check(cw->xRange().upper <= maxIdx + 1e-3, "X-axis upper bound must not exceed maxIndex");

        // 5. Test Playback controls (play, pause, resume, seek, stop)
        win.setCurrentFile(wavA);
        waitForEvents(50);
        ControlPanel* cp = win.controlPanel();
        ok &= check(cp != nullptr, "ControlPanel must exist");

        emit cp->playAudioRequested(wavA, 0.0);
        waitForEvents(50);

        // Simulate playback state update to 25%
        PlaybackState psPlaying{true, 0.25f, 1.0f};
        cp->setPlaybackState(psPlaying, 4000.0f);
        waitForEvents(20);

        // Test pause
        emit cp->pauseRequested();
        waitForEvents(20);
        PlaybackState psPaused{false, 0.25f, 1.0f};
        cp->setPlaybackState(psPaused, 4000.0f);
        waitForEvents(20);

        // Test resume
        emit cp->resumeRequested();
        waitForEvents(20);

        // Test seek to 50%
        emit cp->seekRequested(0.5);
        waitForEvents(20);
        PlaybackState psSeeked{false, 0.5f, 1.0f};
        cp->setPlaybackState(psSeeked, 8000.0f);
        waitForEvents(20);

        // Test stop
        emit cp->stopRequested();
        waitForEvents(20);
        PlaybackState psStopped{false, 0.0f, 1.0f};
        cp->setPlaybackState(psStopped, 0.0f);
        waitForEvents(20);
    }

    QFile::remove(wavA);
    QFile::remove(wavB);

    if (ok) {
        std::cout << "All UI reactivity and zoom constraint tests passed!" << std::endl;
        return 0;
    }
    return 1;
}
