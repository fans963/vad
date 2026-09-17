#include "MainWindow.h"
#include "../ui/panels/HomePanel.h"
#include "../speaker/SpeakerDialog.h"

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
#include <QMessageBox>
#include <QActionGroup>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(QStringLiteral("MyWave - 语音波形与特征分析"));
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
        m_chartWidget->setSeriesVisible(fp, dt, vis);
        m_engine->setChartVisibility(fp, dt, vis);
    });
    connect(m_controlPanel, &ControlPanel::colorChanged,
            this, [this](const QString& fp, DataType dt, const QColor& color) {
        m_chartWidget->setSeriesColor(fp, dt, color);
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
    connect(m_chartWidget, &ChartWidget::selectionRangeChanged, this,
            [this](double first, double last) {
        statusBar()->showMessage(QStringLiteral("已选择样本 %1 — %2（%3 个采样点）")
                                 .arg(first, 0, 'f', 0).arg(last, 0, 'f', 0)
                                 .arg(last - first, 0, 'f', 0));
    });
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
    toolbar->addAction(QStringLiteral("打开..."), this, &MainWindow::onOpenFile);
    toolbar->addSeparator();
    m_fileSelector = new QComboBox;
    m_fileSelector->setMinimumWidth(200);
    m_fileSelector->setToolTip(QStringLiteral("Select audio file for analysis"));
    toolbar->addWidget(m_fileSelector);
    toolbar->addSeparator();
    auto* addCurveBtn = new QPushButton(QStringLiteral("+ 添加分析曲线"));
    auto* addCurveMenu = new QMenu(addCurveBtn);
    struct Entry { QString label; DataType dt; };
    for (const auto& [l, d] : QVector<Entry>{
        {QStringLiteral("语音波形"),                 DataType::Audio},
        {QStringLiteral("左声道波形"),               DataType::AudioLeft},
        {QStringLiteral("右声道波形"),               DataType::AudioRight},
        {QStringLiteral("幅度谱 (FFT)"),            DataType::Spectrum},
        {QStringLiteral("功率谱"),                   DataType::PowerSpectrum},
        {QStringLiteral("对数功率谱"),               DataType::LogSpectrum},
        {QStringLiteral("倒谱"),                     DataType::Cepstrum},
        {QStringLiteral("频谱的频谱（二次 FFT）"),    DataType::SpectrumFFT},
        {QStringLiteral("Mel 频谱"),                DataType::MelSpectrum},
        {QStringLiteral("MFCC"),                    DataType::Mfcc},
        {QStringLiteral("短时能量"),                 DataType::Energy},
        {QStringLiteral("短时平均振幅"),             DataType::AvgAmplitude},
        {QStringLiteral("短时过零率"),               DataType::ZeroCrossingRate},
        {QStringLiteral("修正自相关函数"),             DataType::AutoCorrelation},
        {QStringLiteral("LPC 系数"),                DataType::Lpc},
        {QStringLiteral("LPCC"),                    DataType::Lpcc},
        {QStringLiteral("LPC 合成语音"),             DataType::LpcReconstructed},
        {QStringLiteral("ACF 基音"),                DataType::PitchAcf},
        {QStringLiteral("AMDF 基音"),               DataType::PitchAmdf},
        {QStringLiteral("倒谱法基音"),                 DataType::PitchCep},
        {QStringLiteral("参考程序端点检测"),         DataType::Vad},
        {QStringLiteral("倒谱端点检测"),             DataType::CepstralVad},
        {QStringLiteral("语谱图"),                   DataType::Spectrogram},
    }) { addCurveMenu->addAction(l, [this, d] { addCurveForSelected(d); }); }
    addCurveBtn->setMenu(addCurveMenu);
    toolbar->addWidget(addCurveBtn);
    addToolBar(Qt::TopToolBarArea, toolbar);

    // ── Resizable chart and lower-panel workspace ──────────────────────
    m_chartWidget = new ChartWidget;
    m_controlPanel = new ControlPanel;
    m_controlPanel->setChartWidget(m_chartWidget);

    // ── Bottom panel: stacked pages ─────────────────────────────────────
    m_panelStack = new QStackedWidget;
    m_panelStack->addWidget(new HomePanel);
    m_infoPanel = new InfoPanel;
    m_panelStack->addWidget(m_infoPanel);
    m_panelStack->addWidget(m_controlPanel);

    auto* contentSplitter = new QSplitter(Qt::Vertical, central);
    contentSplitter->setChildrenCollapsible(false);
    contentSplitter->setHandleWidth(6);
    contentSplitter->addWidget(m_chartWidget);
    contentSplitter->addWidget(m_panelStack);
    contentSplitter->setStretchFactor(0, 3);
    contentSplitter->setStretchFactor(1, 1);
    contentSplitter->setSizes({650, 250});
    contentSplitter->setStyleSheet(QStringLiteral(
        "QSplitter::handle:vertical {"
        "  background: palette(mid);"
        "  margin: 2px 0;"
        "}"));

    m_chartWidget->setMinimumHeight(180);
    m_panelStack->setMinimumHeight(120);
    mainLayout->addWidget(contentSplitter, 1);

    // ── Navigation bar (3 tabs like Flutter) ────────────────────────────
    auto* navBar = new QToolBar(QStringLiteral("Pages"));
    navBar->setMovable(false);
    navBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    QAction* homeAct = navBar->addAction(QStringLiteral("关于"));
    QAction* infoAct = navBar->addAction(QStringLiteral("文件信息"));
    QAction* ctrlAct = navBar->addAction(QStringLiteral("操作面板"));

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
    setPage(2); // 参考程序默认显示底部操作面板

    addToolBar(Qt::BottomToolBarArea, navBar);
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu(QStringLiteral("文件(&F)"));
    fileMenu->addAction(QStringLiteral("打开多个文件..."), QKeySequence::Open,
                        this, &MainWindow::onOpenFile);
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("当前音频选区另存为..."),
                        this, &MainWindow::saveCurrentSelection);
    fileMenu->addAction(QStringLiteral("导出当前图表..."), this, &MainWindow::exportChart);
    fileMenu->addSeparator();
    fileMenu->addAction(QStringLiteral("退出"), QKeySequence::Quit,
                        qApp, &QApplication::quit);

    auto addCurveMenu = [this](QMenu* menu,
                              std::initializer_list<std::pair<const char*, DataType>> entries) {
        for (const auto& [label, type] : entries) {
            auto* action = menu->addAction(QString::fromUtf8(label));
            action->setCheckable(true);
            connect(action, &QAction::toggled, this,
                    [this, type](bool checked) { toggleCurveForSelected(type, checked); });
        }
    };

    auto* timeMenu = menuBar()->addMenu(QStringLiteral("时域分析"));
    addCurveMenu(timeMenu, {
        {"短时能量", DataType::Energy}, {"短时过零率", DataType::ZeroCrossingRate},
        {"平均振幅", DataType::AvgAmplitude}, {"修正自相关函数", DataType::AutoCorrelation},
        {"ACF 基音周期", DataType::PitchAcf}, {"AMDF 基音周期", DataType::PitchAmdf},
        {"倒谱法基音周期", DataType::PitchCep},
    });

    auto* frequencyMenu = menuBar()->addMenu(QStringLiteral("频域分析"));
    addCurveMenu(frequencyMenu, {
        {"幅度谱", DataType::Spectrum}, {"功率谱", DataType::PowerSpectrum},
        {"对数功率谱", DataType::LogSpectrum}, {"倒谱", DataType::Cepstrum},
        {"频谱的频谱", DataType::SpectrumFFT}, {"Mel 频谱", DataType::MelSpectrum},
        {"MFCC", DataType::Mfcc}, {"LPC", DataType::Lpc}, {"LPCC", DataType::Lpcc},
        {"LPC 合成语音", DataType::LpcReconstructed},
    });
    frequencyMenu->addSeparator();
    frequencyMenu->addAction(QStringLiteral("保存有效帧 MFCC..."),
                             this, &MainWindow::exportEffectiveMfcc);

    auto* endpointMenu = menuBar()->addMenu(QStringLiteral("端点检测"));
    addCurveMenu(endpointMenu, {
        {"能量 + 过零率双门限（参考程序）", DataType::Vad},
        {"倒谱端点检测", DataType::CepstralVad},
    });

    auto* displayMenu = menuBar()->addMenu(QStringLiteral("显示"));
    addCurveMenu(displayMenu, {
        {"语音波形", DataType::Audio}, {"左声道", DataType::AudioLeft},
        {"右声道", DataType::AudioRight}, {"语谱图", DataType::Spectrogram},
    });
    displayMenu->addAction(QStringLiteral("取消选区"), m_chartWidget, &ChartWidget::clearSelection);

    auto* frameMenu = menuBar()->addMenu(QStringLiteral("帧长设置"));
    auto* frameGroup = new QActionGroup(this);
    for (int size : {256, 512, 1024}) {
        auto* action = frameMenu->addAction(QString::number(size));
        action->setCheckable(true);
        action->setChecked(size == m_controlPanel->globalFrameSize());
        frameGroup->addAction(action);
        connect(action, &QAction::triggered, this,
                [this, size] { onGlobalFrameSizeChanged(size); });
    }

    auto* speakerMenu = menuBar()->addMenu(QStringLiteral("说话人"));
    speakerMenu->addAction(QStringLiteral("说话人识别 / 确认..."), this, [this] {
        SpeakerDialog dialog(this);
        dialog.exec();
    });

    auto* viewMenu = menuBar()->addMenu(QStringLiteral("视图(&V)"));
    viewMenu->addAction(QStringLiteral("还原位置"), this, [this]() {
        m_chartWidget->setXRange(0, 10000);
        m_chartWidget->setYRange(-0.5, 0.5);
    });

    auto* helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
    helpMenu->addAction(QStringLiteral("关于 MyWave"), this, [this]() {
        QMessageBox::about(this, QStringLiteral("关于 MyWave"),
            QStringLiteral("MyWave 1.0\n\nQt 6 跨平台移植版\n"
                           "功能与 MyWave VS2010 参考工程对齐。"));
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

void MainWindow::saveCurrentSelection() {
    const QString source = currentFilePath();
    if (source.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("文件未打开"),
                             QStringLiteral("请先打开并选择一个音频文件。"));
        return;
    }
    QString output = QFileDialog::getSaveFileName(
        this, QStringLiteral("当前音频选区另存为"),
        QFileInfo(source).completeBaseName() + QStringLiteral("_selection.wav"),
        QStringLiteral("WAV 音频 (*.wav)"));
    if (output.isEmpty()) return;
    if (!output.endsWith(QStringLiteral(".wav"), Qt::CaseInsensitive)) output += QStringLiteral(".wav");

    const auto range = m_chartWidget->selectionRange();
    const uint64_t first = range.first >= 0.0 ? uint64_t(range.first) : 0;
    const uint64_t last = range.second > range.first
        ? uint64_t(range.second) : std::numeric_limits<uint64_t>::max();
    if (!m_engine->saveWavSegment(source, output, first, last))
        QMessageBox::critical(this, QStringLiteral("保存失败"), QStringLiteral("无法保存 WAV 文件。"));
    else
        statusBar()->showMessage(QStringLiteral("已保存：%1").arg(output), 5000);
}

void MainWindow::exportChart() {
    QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出当前图表"), QStringLiteral("MyWave.png"),
        QStringLiteral("PNG 图片 (*.png);;PDF 文档 (*.pdf)"));
    if (path.isEmpty()) return;
    const bool ok = path.endsWith(QStringLiteral(".pdf"), Qt::CaseInsensitive)
        ? m_chartWidget->savePdf(path) : m_chartWidget->savePng(path);
    if (!ok)
        QMessageBox::critical(this, QStringLiteral("导出失败"), QStringLiteral("无法写入所选文件。"));
}

void MainWindow::exportEffectiveMfcc() {
    const QString source = currentFilePath();
    if (source.isEmpty()) return;
    QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("保存有效帧 MFCC"),
        QFileInfo(source).completeBaseName() + QStringLiteral("_effective_mfcc.tsv"),
        QStringLiteral("TSV 数据 (*.tsv *.txt)"));
    if (path.isEmpty()) return;
    if (!m_engine->exportEffectiveMfcc(source, path))
        QMessageBox::critical(this, QStringLiteral("导出失败"), QStringLiteral("无法写入 MFCC 数据。"));
    else
        statusBar()->showMessage(QStringLiteral("已保存有效帧 MFCC：%1").arg(path), 5000);
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
    if (info.sampleRate == 0 || info.sampleCount == 0) {
        QMessageBox::critical(this, QStringLiteral("打开失败"),
                              QStringLiteral("无法解码音频文件：\n%1").arg(filePath));
        return;
    }
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

void MainWindow::toggleCurveForSelected(DataType dt, bool visible) {
    const auto filePath = currentFilePath();
    if (filePath.isEmpty()) return;
    if (visible) {
        m_engine->setChartVisibility(filePath, dt, true);
        m_engine->addChart(filePath, dt);
    } else {
        m_chartWidget->setSeriesVisible(filePath, dt, false);
        m_engine->setChartVisibility(filePath, dt, false);
    }
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
    m_controlPanel->setGlobalFrameSize(newSize);
    m_engine->setConfig({newSize, m_engine->config().downSamplePointsNum});
    m_chartWidget->setFrameGrid(newSize);
}

void MainWindow::onPlayAudio(const QString& filePath, double startFraction) {
    const auto range = m_chartWidget->selectionRange();
    if (range.first >= 0.0 && range.second > range.first)
        m_engine->playAudioRange(filePath, uint64_t(range.first), uint64_t(range.second));
    else
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
