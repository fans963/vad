#include "ControlPanel.h"
#include "../ChartWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QColorDialog>
#include <QMessageBox>

ControlPanel::ControlPanel(QWidget* parent) : QWidget(parent) {
    setupUi();

    m_debounce = new QTimer(this);
    m_debounce->setSingleShot(true);
    m_debounce->setInterval(50);
    connect(m_debounce, &QTimer::timeout, this, &ControlPanel::emitViewRangeChanged);
}

void ControlPanel::setChartWidget(ChartWidget* cw) { m_chartWidget = cw; }

void ControlPanel::setupUi() {
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);

    auto* w = new QWidget;
    scrollArea->setWidget(w);
    auto* layout = new QVBoxLayout(w);

    // ── Series selector ──────────────────────────────────────────────────
    auto* seriesGroup = new QGroupBox(QStringLiteral("曲线管理"));
    auto* sgLayout = new QVBoxLayout(seriesGroup);

    auto* selRow = new QHBoxLayout;
    selRow->addWidget(new QLabel(QStringLiteral("选择：")));
    m_seriesCombo = new QComboBox;
    m_seriesCombo->setMinimumWidth(150);
    selRow->addWidget(m_seriesCombo, 1);
    sgLayout->addLayout(selRow);

    auto* btnRow = new QHBoxLayout;
    m_colorBtn = new QPushButton(QStringLiteral("颜色"));
    m_colorBtn->setMaximumWidth(60);
    btnRow->addWidget(m_colorBtn);
    m_visibleBtn = new QPushButton(QStringLiteral("显示/隐藏"));
    m_visibleBtn->setMaximumWidth(80);
    btnRow->addWidget(m_visibleBtn);
    m_playBtn = new QPushButton(QStringLiteral("播放选区"));
    btnRow->addWidget(m_playBtn);
    m_stopBtn = new QPushButton(QStringLiteral("停止"));
    btnRow->addWidget(m_stopBtn);
    m_deleteBtn = new QPushButton(QStringLiteral("删除曲线"));
    m_deleteBtn->setStyleSheet("color: red;");
    btnRow->addWidget(m_deleteBtn);
    sgLayout->addLayout(btnRow);
    layout->addWidget(seriesGroup);

    // ── Axis controls ─────────────────────────────────────────────────────
    auto* axisGroup = new QGroupBox(QStringLiteral("坐标轴控制"));
    auto* agLayout = new QVBoxLayout(axisGroup);

    // X-axis
    agLayout->addWidget(new QLabel(QStringLiteral("X 轴显示位置：")));
    m_xPosSlider = new QSlider(Qt::Horizontal);
    m_xPosSlider->setRange(0, 1000);
    m_xPosSlider->setValue(500);
    agLayout->addWidget(m_xPosSlider);

    auto* xzRow = new QHBoxLayout;
    xzRow->addWidget(new QLabel(QStringLiteral("X 轴缩放：")));
    m_xZoomSlider = new QSlider(Qt::Horizontal);
    m_xZoomSlider->setRange(1, 1000);
    m_xZoomSlider->setValue(1000);
    xzRow->addWidget(m_xZoomSlider, 1);
    m_xZoomEdit = new QLineEdit("100%");
    m_xZoomEdit->setMaximumWidth(60);
    m_xZoomEdit->setReadOnly(true);
    xzRow->addWidget(m_xZoomEdit);
    agLayout->addLayout(xzRow);

    // Y-axis
    auto* yzRow = new QHBoxLayout;
    yzRow->addWidget(new QLabel(QStringLiteral("Y 轴缩放：")));
    m_yZoomSlider = new QSlider(Qt::Horizontal);
    m_yZoomSlider->setRange(10, 1000);
    m_yZoomSlider->setValue(100);
    yzRow->addWidget(m_yZoomSlider, 1);
    m_yZoomEdit = new QLineEdit("1.0x");
    m_yZoomEdit->setMaximumWidth(60);
    m_yZoomEdit->setReadOnly(true);
    yzRow->addWidget(m_yZoomEdit);
    agLayout->addLayout(yzRow);

    agLayout->addWidget(new QLabel(QStringLiteral("Y 轴显示位置：")));
    m_yPosSlider = new QSlider(Qt::Horizontal);
    m_yPosSlider->setRange(0, 1000);
    m_yPosSlider->setValue(500);
    agLayout->addWidget(m_yPosSlider);

    m_viewInfoLabel = new QLineEdit;
    m_viewInfoLabel->setReadOnly(true);
    m_viewInfoLabel->setStyleSheet("font-size: 11px;");
    agLayout->addWidget(m_viewInfoLabel);

    layout->addWidget(axisGroup);

    // ── Frame size ────────────────────────────────────────────────────────
    auto* frameGroup = new QGroupBox(QStringLiteral("全局帧长"));
    auto* fgLayout = new QHBoxLayout(frameGroup);
    m_frameSizeSlider = new QSlider(Qt::Horizontal);
    m_frameSizeSlider->setRange(6, 12); // 2^6=64 to 2^12=4096 (exponential)
    m_frameSizeSlider->setValue(9);     // 512 (reference program default)
    fgLayout->addWidget(m_frameSizeSlider, 1);
    m_frameSizeLabel = new QLabel(QStringLiteral("512"));
    m_frameSizeLabel->setFixedWidth(48);
    m_frameSizeLabel->setAlignment(Qt::AlignCenter);
    m_frameSizeLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: palette(window); border: 1px solid palette(mid); "
        "border-radius: 4px; padding: 2px; font-weight: bold; }"));
    fgLayout->addWidget(m_frameSizeLabel);
    layout->addWidget(frameGroup);

    // ── VAD ───────────────────────────────────────────────────────────────
    auto* vadGroup = new QGroupBox(QStringLiteral("VAD"));
    auto* vgLayout = new QVBoxLayout(vadGroup);

    auto* vadRow = new QHBoxLayout;
    m_vadAlgoCombo = new QComboBox;
    vadRow->addWidget(m_vadAlgoCombo, 1);
    m_vadRunBtn = new QPushButton(QStringLiteral("执行端点检测"));
    vadRow->addWidget(m_vadRunBtn);
    vgLayout->addLayout(vadRow);

    m_vadParamsContainer = new QWidget;
    m_vadParamsContainer->setLayout(new QVBoxLayout);
    vgLayout->addWidget(m_vadParamsContainer);

    layout->addWidget(vadGroup);
    layout->addStretch();

    // ── Connections ───────────────────────────────────────────────────────
    connect(m_xZoomSlider, &QSlider::valueChanged, this, &ControlPanel::onSliderChanged);
    connect(m_xPosSlider, &QSlider::valueChanged, this, &ControlPanel::onSliderChanged);
    connect(m_yZoomSlider, &QSlider::valueChanged, this, &ControlPanel::onSliderChanged);
    connect(m_yPosSlider, &QSlider::valueChanged, this, &ControlPanel::onSliderChanged);
    connect(m_seriesCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ControlPanel::onSeriesSelectorChanged);
    connect(m_colorBtn, &QPushButton::clicked, this, &ControlPanel::onShowColorPicker);
    connect(m_visibleBtn, &QPushButton::clicked, this, &ControlPanel::onToggleVisibility);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ControlPanel::onDeleteSelected);
    connect(m_playBtn, &QPushButton::clicked, this, &ControlPanel::onPlaySelected);
    connect(m_stopBtn, &QPushButton::clicked, this, &ControlPanel::onStop);
    connect(m_vadAlgoCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ControlPanel::onVadAlgorithmChanged);
    connect(m_vadRunBtn, &QPushButton::clicked, this, &ControlPanel::onRunVad);
    connect(m_frameSizeSlider, &QSlider::valueChanged, this, [this](int v) {
        int fs = 1 << v;
        m_frameSizeLabel->setText(QString::number(fs));
        updateXZoomRange();
        emit globalFrameSizeChanged(fs);
    });
}

// ── Accessors ───────────────────────────────────────────────────────────────

double ControlPanel::xZoom() const { return m_xZoomSlider->value() / 1000.0; }
double ControlPanel::xPosition() const { return m_xPosSlider->value() / 1000.0; }
double ControlPanel::yZoom() const { return m_yZoomSlider->value() / 100.0; }
double ControlPanel::yPosition() const { return m_yPosSlider->value() / 1000.0; }

double ControlPanel::xViewMin() const {
    double viewable = m_maxIndex * xZoom();
    double slack = m_maxIndex - viewable;
    double offset = (xPosition() - 0.5) * slack;
    double center = m_maxIndex * 0.5 + offset;
    return std::max(0.0, center - viewable / 2);
}
double ControlPanel::xViewMax() const {
    double viewable = m_maxIndex * xZoom();
    double slack = m_maxIndex - viewable;
    double offset = (xPosition() - 0.5) * slack;
    double center = m_maxIndex * 0.5 + offset;
    return std::min((double)m_maxIndex, center + viewable / 2);
}
double ControlPanel::yViewMin() const {
    double range = m_yAutoMax - m_yAutoMin;
    double viewable = range / yZoom();
    double slack = range - viewable;
    double offset = (yPosition() - 0.5) * slack;
    double center = m_yAutoMin + range * 0.5 + offset;
    return center - viewable / 2;
}
double ControlPanel::yViewMax() const {
    double range = m_yAutoMax - m_yAutoMin;
    double viewable = range / yZoom();
    double slack = range - viewable;
    double offset = (yPosition() - 0.5) * slack;
    double center = m_yAutoMin + range * 0.5 + offset;
    return center + viewable / 2;
}

int ControlPanel::globalFrameSize() const { return 1 << m_frameSizeSlider->value(); }

void ControlPanel::setGlobalFrameSize(int size) {
    int power = 8;
    while ((1 << power) < size && power < 10) ++power;
    const QSignalBlocker blocker(m_frameSizeSlider);
    m_frameSizeSlider->setValue(power);
    m_frameSizeLabel->setText(QString::number(1 << power));
    updateXZoomRange();
}

void ControlPanel::setChartMaxIndex(float idx) {
    m_maxIndex = idx;
    updateXZoomRange();
    updateViewInfo();
    if (m_chartWidget)
        m_chartWidget->setXRange(xViewMin(), xViewMax());
}

void ControlPanel::updateXZoomRange() {
    if (m_maxIndex <= 0 || m_frameSizeSlider->value() <= 0) return;
    int frameSize = 1 << m_frameSizeSlider->value();
    // min zoom = 5*frameSize visible on screen
    int minVal = std::max(1, (int)std::ceil(5000.0 * frameSize / m_maxIndex));
    minVal = std::min(minVal, 1000);
    m_xZoomSlider->setRange(minVal, 1000);
    if (m_xZoomSlider->value() < minVal)
        m_xZoomSlider->setValue(minVal);
}
void ControlPanel::setAutoYRange(float yMin, float yMax) {
    m_yAutoMin = yMin; m_yAutoMax = yMax;
    updateViewInfo();
    if (m_chartWidget)
        m_chartWidget->setYRange(yViewMin(), yViewMax());
}

// ── Slider updates ──────────────────────────────────────────────────────────

void ControlPanel::onSliderChanged() {
    m_xZoomEdit->setText(QStringLiteral("%1%").arg((int)(xZoom() * 100)));
    m_yZoomEdit->setText(QStringLiteral("%1x").arg(yZoom(), 0, 'f', 1));
    updateViewInfo();

    // Apply to chart widget
    if (m_chartWidget) {
        m_chartWidget->setXRange(xViewMin(), xViewMax());
        m_chartWidget->setYRange(yViewMin(), yViewMax());
    }

    m_debounce->start();
}

void ControlPanel::emitViewRangeChanged() { emit viewRangeChanged(); }

void ControlPanel::updateViewInfo() {
    m_viewInfoLabel->setText(
        QStringLiteral("X: %1-%2  |  Y: %3-%4  |  Total: %5")
            .arg(xViewMin(), 0, 'f', 0).arg(xViewMax(), 0, 'f', 0)
            .arg(yViewMin(), 0, 'f', 3).arg(yViewMax(), 0, 'f', 3)
            .arg(m_maxIndex, 0, 'f', 0));
}

// ── Series management ───────────────────────────────────────────────────────

void ControlPanel::refreshSeriesSelector() {
    m_seriesCombo->blockSignals(true);
    m_seriesCombo->clear();
    if (m_chartWidget) {
        auto keys = m_chartWidget->allSeriesKeys();
        m_seriesCombo->addItem(QStringLiteral("（无）"), QString());
        for (const auto& k : keys) {
            auto [fp, dt] = parseSeriesKey(k);
            QString label = QStringLiteral("%1 - %2").arg(fp.split('/').last(),
                                                           dataTypeDisplayName(dt));
            m_seriesCombo->addItem(label, k);
        }
    }
    m_seriesCombo->blockSignals(false);
}

void ControlPanel::onSeriesSelectorChanged(int index) {
    if (index < 0) return;
    QString key = m_seriesCombo->itemData(index).toString();
    emit selectionChanged(key);
    if (m_chartWidget) m_chartWidget->setSelectedKey(key);
}

void ControlPanel::onShowColorPicker() {
    QString key = m_seriesCombo->currentData().toString();
    if (key.isEmpty()) return;
    auto [fp, dt] = parseSeriesKey(key);
    QColor c = QColorDialog::getColor(Qt::blue, this, QStringLiteral("选择曲线颜色"));
    if (c.isValid()) {
        emit colorChanged(fp, dt, c);
        m_colorBtn->setStyleSheet(QStringLiteral("background-color: %1;").arg(c.name()));
    }
}

void ControlPanel::onToggleVisibility() {
    QString key = m_seriesCombo->currentData().toString();
    if (key.isEmpty()) return;
    auto [fp, dt] = parseSeriesKey(key);
    bool vis = m_chartWidget ? m_chartWidget->isVisible(fp, dt) : true;
    emit visibilityToggled(fp, dt, !vis);
}

void ControlPanel::onDeleteSelected() {
    QString key = m_seriesCombo->currentData().toString();
    if (key.isEmpty()) return;
    auto [fp, dt] = parseSeriesKey(key);
    auto reply = QMessageBox::question(this, QStringLiteral("确认删除"),
        QStringLiteral("删除 %2 的 %1 曲线？").arg(dataTypeDisplayName(dt),
                                                fp.split('/').last()));
    if (reply == QMessageBox::Yes)
        emit deleteSeriesRequested(fp, dt);
}

void ControlPanel::onPlaySelected() {
    QString key = m_seriesCombo->currentData().toString();
    if (key.isEmpty()) return;
    auto [fp, dt] = parseSeriesKey(key);
    emit playAudioRequested(fp, xPosition());
}

void ControlPanel::onStop() { emit stopRequested(); }

// ── VAD ─────────────────────────────────────────────────────────────────────

void ControlPanel::setVadAlgorithms(const QStringList& names, const QString& current) {
    m_vadAlgoCombo->blockSignals(true);
    m_vadAlgoCombo->clear();
    for (const auto& n : names) {
        m_vadAlgoCombo->addItem(n);
        if (n == current) m_vadAlgoCombo->setCurrentIndex(m_vadAlgoCombo->count() - 1);
    }
    m_vadAlgoCombo->blockSignals(false);
}

void ControlPanel::setVadParams(const QVector<VadParamDef>& params) {
    // Delete old child widgets
    qDeleteAll(m_vadParamsContainer->findChildren<QWidget*>(
        QString(), Qt::FindDirectChildrenOnly));
    // Delete old layout
    delete m_vadParamsContainer->layout();
    auto* pl = new QVBoxLayout(m_vadParamsContainer);
    m_vadParamSliders.clear();

    for (const auto& p : params) {
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(p.label));

        auto* slider = new QSlider(Qt::Horizontal);
        if (p.kind == VadParamKind::Int) {
            slider->setRange((int)p.min, (int)p.max);
            slider->setValue((int)p.value);
        } else {
            slider->setRange(p.min / p.step, p.max / p.step);
            slider->setValue(p.value / p.step);
        }
        row->addWidget(slider, 1);

        auto* valLabel = new QLabel;
        if (p.kind == VadParamKind::Int)
            valLabel->setText(QString::number((int)p.value));
        else
            valLabel->setText(QString::number(p.value, 'f', 3));
        row->addWidget(valLabel);

        connect(slider, &QSlider::valueChanged, this,
                [this, p, slider, valLabel](int v) {
            float val = p.kind == VadParamKind::Int
                ? (float)v
                : v * p.step;
            if (p.kind == VadParamKind::Int)
                valLabel->setText(QString::number((int)val));
            else
                valLabel->setText(QString::number(val, 'f', 3));
            emit vadParamChanged(p.key, val);
        });

        pl->addLayout(row);
        m_vadParamSliders.append({p.key, slider});
    }
}

void ControlPanel::onVadAlgorithmChanged(int index) {
    if (index >= 0)
        emit vadAlgorithmChanged(m_vadAlgoCombo->currentText());
}

void ControlPanel::onVadParamChanged() { /* handled inline */ }

void ControlPanel::onRunVad() {
    QString key = m_seriesCombo->currentData().toString();
    // Fallback: use first non-empty key from combo if none selected
    if (key.isEmpty() && m_seriesCombo->count() > 1) {
        key = m_seriesCombo->itemData(1).toString(); // index 0 is "(none)"
    }
    if (key.isEmpty()) return;
    auto [fp, dt] = parseSeriesKey(key);
    qDebug() << "[CTRL] run VAD on:" << fp;
    emit vadRunRequested(fp);
}

void ControlPanel::setPlaybackState(const PlaybackState& state, float chartPos) {
    Q_UNUSED(state); Q_UNUSED(chartPos);
    // Update playback display if play bar is added later
}
