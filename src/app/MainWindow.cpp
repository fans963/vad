#include "MainWindow.h"
#include "../ui/panels/HomePanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QComboBox>
#include <QMenu>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("VAD - Voice Activity Detection"));
    resize(1400, 900);
    setAcceptDrops(true);

    m_engine = new AudioEngine(this);
    setupUi();
    setupMenus();
    wireSignals();
    initState();

    statusBar()->showMessage(QStringLiteral("Ready"));
}

MainWindow::~MainWindow() {}

void MainWindow::initState() {
    // ── VAD state (like Flutter refreshVadState) ────────────────────────
    m_controlPanel->setVadAlgorithms(m_engine->listVadAlgorithms(),
                                      m_engine->currentVadName());
    m_controlPanel->setVadParams(m_engine->vadParams());
    // Initial frame grid
    m_chartWidget->setFrameGrid(m_controlPanel->globalFrameSize());
}

void MainWindow::wireSignals() {
    // ── Engine → UI ─────────────────────────────────────────────────────
    connect(m_engine, &AudioEngine::chartEvent,
            this, &MainWindow::onChartEvent);

    // ── Control panel → Engine ──────────────────────────────────────────
    connect(m_controlPanel, &ControlPanel::viewRangeChanged,
            this, &MainWindow::onViewRangeChanged);
    connect(m_controlPanel, &ControlPanel::globalFrameSizeChanged,
            this, &MainWindow::onGlobalFrameSizeChanged);
    connect(m_controlPanel, &ControlPanel::playAudioRequested,
            this, &MainWindow::onPlayAudio);
    connect(m_controlPanel, &ControlPanel::pauseRequested,
            this, [this]() { m_engine->pauseAudio(); });
    connect(m_controlPanel, &ControlPanel::resumeRequested,
            this, [this]() { m_engine->resumeAudio(); });
    connect(m_controlPanel, &ControlPanel::stopRequested,
            this, &MainWindow::onStopAudio);
    connect(m_controlPanel, &ControlPanel::seekRequested,
            this, [this](double f) { m_engine->seekAudio(f); });
    connect(m_controlPanel, &ControlPanel::speedChanged,
            this, [this](float m) { m_engine->setPlaybackSpeed(m); });
    connect(m_controlPanel, &ControlPanel::vadAlgorithmChanged,
            this, &MainWindow::onVadAlgorithmChanged);
    connect(m_controlPanel, &ControlPanel::vadParamChanged,
            this, &MainWindow::onVadParamChanged);
    connect(m_controlPanel, &ControlPanel::vadRunRequested,
            this, &MainWindow::onRunVad);

    // ── Series management → Engine ──────────────────────────────────────
    connect(m_controlPanel, &ControlPanel::deleteSeriesRequested,
            this, [this](const QString& fp, DataType dt) {
        m_engine->removeChart(fp, dt);
    });
    connect(m_controlPanel, &ControlPanel::visibilityToggled,
            this, [this](const QString& fp, DataType dt, bool vis) {
        m_engine->setChartVisibility(fp, dt, vis);
    });
    connect(m_controlPanel, &ControlPanel::colorChanged,
            this, [this](const QString& fp, DataType dt, const QColor&) {
        // Color is handled client-side in ChartWidget meta, trigger rebuild
        m_chartWidget->rebuildSeries();
        Q_UNUSED(fp); Q_UNUSED(dt);
    });
    connect(m_controlPanel, &ControlPanel::selectionChanged,
            this, [this](const QString& key) {
        m_chartWidget->setSelectedKey(key);
    });

    // ── Chart width → adaptive downsampling target ─────────────────────
    connect(m_chartWidget, &ChartWidget::plotWidthChanged, this,
            [this](int w) {
        m_engine->setDownSamplePointsNum(std::max(200, w * 2));
    });
    // Init with current width
    m_engine->setDownSamplePointsNum(
        std::max(200, m_chartWidget->plotWidth() * 2));
}

void MainWindow::setupUi() {
    auto* central = new QWidget;
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(4, 4, 4, 0);
    mainLayout->setSpacing(0);

    // ── Toolbar: file selector + analysis ────────────────────────────────
    auto* toolbar = new QToolBar(QStringLiteral("Main"));
    toolbar->setMovable(false);
    toolbar->addAction(QStringLiteral("Open..."), this, &MainWindow::onOpenFile);
    toolbar->addSeparator();
    m_fileSelector = new QComboBox;
    m_fileSelector->setMinimumWidth(200);
    m_fileSelector->setToolTip(QStringLiteral("Select audio file for analysis"));
    toolbar->addWidget(m_fileSelector);
    toolbar->addSeparator();
    auto* addCurveBtn = new QPushButton(QStringLiteral("+ Add Curve"));
    auto* addCurveMenu = new QMenu(addCurveBtn);
    struct Entry { QString label; DataType dt; };
    for (const auto& [l, d] : QVector<Entry>{
        {QStringLiteral("Audio Waveform"),           DataType::Audio},
        {QStringLiteral("Spectrum (FFT)"),           DataType::Spectrum},
        {QStringLiteral("Spectrum of Spectrum"),     DataType::SpectrumFFT},
        {QStringLiteral("Energy (dB)"),              DataType::Energy},
        {QStringLiteral("Avg Amplitude"),            DataType::AvgAmplitude},
        {QStringLiteral("Zero-Crossing Rate"),       DataType::ZeroCrossingRate},
        {QStringLiteral("Auto-Correlation"),         DataType::AutoCorrelation},
        {QStringLiteral("LPC Coefficients"),         DataType::Lpc},
        {QStringLiteral("LPCC"),                     DataType::Lpcc},
        {QStringLiteral("Pitch (ACF)"),              DataType::PitchAcf},
        {QStringLiteral("Pitch (AMDF)"),             DataType::PitchAmdf},
        {QStringLiteral("Pitch (Cepstral)"),         DataType::PitchCep},
        {QStringLiteral("VAD (Energy)"),             DataType::Vad},
        {QStringLiteral("VAD (ZCR)"),                DataType::Vad},
        {QStringLiteral("VAD (Cepstral)"),           DataType::CepstralVad},
        {QStringLiteral("Spectrogram"),              DataType::Spectrogram},
    }) { addCurveMenu->addAction(l, [this, d] { addCurveForSelected(d); }); }
    addCurveBtn->setMenu(addCurveMenu);
    toolbar->addWidget(addCurveBtn);
    addToolBar(Qt::TopToolBarArea, toolbar);

    // ── Chart widget (fill space) ───────────────────────────────────────
    m_chartWidget = new ChartWidget;
    m_controlPanel = new ControlPanel;
    m_controlPanel->setChartWidget(m_chartWidget);
    mainLayout->addWidget(m_chartWidget, 1);

    // ── Bottom panel: stacked pages ─────────────────────────────────────
    m_panelStack = new QStackedWidget;
    m_panelStack->addWidget(new HomePanel);
    m_infoPanel = new InfoPanel;
    m_panelStack->addWidget(m_infoPanel);
    m_panelStack->addWidget(m_controlPanel);
    mainLayout->addWidget(m_panelStack);

    // ── Navigation bar (3 tabs like Flutter) ────────────────────────────
    auto* navBar = new QToolBar(QStringLiteral("Pages"));
    navBar->setMovable(false);
    navBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    QAction* homeAct = navBar->addAction(QStringLiteral("Home"));
    QAction* infoAct = navBar->addAction(QStringLiteral("Info"));
    QAction* ctrlAct = navBar->addAction(QStringLiteral("Control"));

    auto setPage = [this, homeAct, infoAct, ctrlAct](int idx) {
        m_panelStack->setCurrentIndex(idx);
        for (auto* a : {homeAct, infoAct, ctrlAct}) a->setChecked(false);
        switch (idx) {
        case 0: homeAct->setChecked(true); break;
        case 1: infoAct->setChecked(true); break;
        case 2: ctrlAct->setChecked(true); break;
        }
    };
    connect(homeAct, &QAction::triggered, this, [=]() { setPage(0); });
    connect(infoAct, &QAction::triggered, this, [=]() { setPage(1); });
    connect(ctrlAct, &QAction::triggered, this, [=]() { setPage(2); });
    setPage(0); // default: home

    addToolBar(Qt::BottomToolBarArea, navBar);
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    fileMenu->addAction(QStringLiteral("&Open..."), QKeySequence::Open,
                        this, &MainWindow::onOpenFile);
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("&Quit"), QKeySequence::Quit,
                        qApp, &QApplication::quit);

    auto* viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    viewMenu->addAction(QStringLiteral("Reset View"), this, [this]() {
        m_chartWidget->setXRange(0, 10000);
        m_chartWidget->setYRange(-0.5, 0.5);
    });

    auto* helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    helpMenu->addAction(QStringLiteral("About"), this, [this]() {
        QMessageBox::about(this, QStringLiteral("VAD"),
            QStringLiteral("Voice Activity Detection Analyzer\n\n"
                           "Qt/C++ implementation based on VAD Flutter & Rust\n"
                           "Developers: Fans963 & 津哥"));
    });
}

// ── File loading ──────────────────────────────────────────────────────────

void MainWindow::onOpenFile() {
    QString filter = QStringLiteral(
        "Audio Files (*.wav *.mp3 *.flac *.aac *.ogg *.m4a);;All Files (*)");
    auto files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("Open Audio File"), QString(), filter);
    for (const auto& f : files)
        loadAudioFile(f);
}

void MainWindow::loadAudioFile(const QString& filePath, const QByteArray& data) {
    QByteArray fileData = data;
    if (fileData.isEmpty()) {
        QFile f(filePath);
        if (!f.open(QIODevice::ReadOnly)) return;
        fileData = f.readAll();
    }

    QFileInfo fi(filePath);
    QString format = fi.suffix().toLower();

    // 1. Decode and add Audio waveform chart
    m_engine->addFile(filePath, fileData, format);

    // Update info panel
    auto info = m_engine->getAudioInfo(filePath);
    m_audioInfoMap[filePath] = info;
    m_infoPanel->setAudioInfoMap(m_audioInfoMap);

    m_controlPanel->refreshSeriesSelector();
    refreshFileSelector();

    statusBar()->showMessage(
        QStringLiteral("Loaded: %1 (%2 Hz, %3 ch)")
            .arg(fi.fileName()).arg(info.sampleRate).arg(info.channels),
        5000);
}

QString MainWindow::currentFilePath() const {
    if (m_fileSelector && m_fileSelector->currentIndex() >= 0)
        return m_fileSelector->currentData().toString();
    return {};
}

void MainWindow::addCurveForSelected(DataType dt) {
    auto fp = currentFilePath();
    if (fp.isEmpty()) return;
    if (dt == DataType::Vad || dt == DataType::CepstralVad) {
        m_engine->removeChart(fp, DataType::Vad);
        m_engine->removeChart(fp, DataType::CepstralVad);
    }
    m_engine->addChart(fp, dt);
}

void MainWindow::refreshFileSelector() {
    if (!m_fileSelector) return;
    QString cur = m_fileSelector->currentData().toString();
    m_fileSelector->blockSignals(true);
    m_fileSelector->clear();
    for (const auto& fp : m_engine->loadedFiles())
        m_fileSelector->addItem(fp.section(QLatin1Char('/'), -1), fp);
    int idx = m_fileSelector->findData(cur);
    if (idx >= 0) m_fileSelector->setCurrentIndex(idx);
    m_fileSelector->blockSignals(false);
}

// ── Drag & drop ──────────────────────────────────────────────────────────

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event) {
    for (const auto& url : event->mimeData()->urls()) {
        if (url.isLocalFile())
            loadAudioFile(url.toLocalFile());
    }
}

// ── Chart events from engine ─────────────────────────────────────────────

void MainWindow::onChartEvent(const ChartEvent& event) {
    m_chartWidget->handleChartEvent(event);

    switch (event.type) {
    case ChartEvent::UpdateMaxIndex:
        m_controlPanel->setChartMaxIndex(event.maxIndex);
        break;
    case ChartEvent::UpdateYRange:
        m_controlPanel->setAutoYRange(event.minY, event.maxY);
        break;
    case ChartEvent::AddChart:
    case ChartEvent::UpdateAllCharts:
    case ChartEvent::RemoveChart:
    case ChartEvent::RemoveAllCharts:
        m_controlPanel->refreshSeriesSelector();
        break;
    case ChartEvent::UpdatePlaybackState:
        m_controlPanel->setPlaybackState(event.playbackState, event.chartPosition);
        break;
    }
}

// ── Slots ────────────────────────────────────────────────────────────────

void MainWindow::onViewRangeChanged() {
    m_engine->setIndexRange(m_controlPanel->xViewMin(),
                             m_controlPanel->xViewMax());
}

void MainWindow::onGlobalFrameSizeChanged(int newSize) {
    m_engine->setConfig({newSize, m_engine->config().downSamplePointsNum});
    m_chartWidget->setFrameGrid(newSize);
}

void MainWindow::onPlayAudio(const QString& filePath, double startFraction) {
    m_engine->playAudio(filePath, startFraction);
}
void MainWindow::onStopAudio() { m_engine->stopAudio(); }

void MainWindow::onVadAlgorithmChanged(const QString& name) {
    m_engine->setVadAlgorithm(name);
    m_controlPanel->setVadParams(m_engine->vadParams());
}

void MainWindow::onVadParamChanged(const QString& key, float value) {
    m_engine->setVadParam(key, value);
}

void MainWindow::onRunVad(const QString& filePath) {
    qDebug() << "[MAIN] onRunVad:" << filePath;
    m_engine->removeChart(filePath, DataType::Vad);
    m_engine->addChart(filePath, DataType::Vad);
}

void MainWindow::onVadRunForCurrentFile() {
    for (const auto& fp : m_engine->loadedFiles()) {
        m_engine->removeChart(fp, DataType::Vad);
        m_engine->addChart(fp, DataType::Vad);
    }
}
