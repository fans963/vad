#include "ControlPanel.h"
#include "../ChartWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
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
    auto* rootLayout = new QVBoxLayout(w);
    rootLayout->setContentsMargins(6, 6, 6, 6);

    auto* dashboard = new QWidget;
    dashboard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto* layout = new QGridLayout(dashboard);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setHorizontalSpacing(8);
    layout->setVerticalSpacing(6);
    layout->setColumnStretch(0, 2); // Series management
    layout->setColumnStretch(1, 4); // Axis controls
    layout->setColumnStretch(2, 1); // Frame size
    layout->setColumnStretch(3, 3); // VAD
    rootLayout->addWidget(dashboard, 0);
    rootLayout->addStretch(1);

    // ── Series selector ──────────────────────────────────────────────────
    auto* seriesGroup = new QGroupBox(QStringLiteral("曲线管理"));
    auto* sgLayout = new QVBoxLayout(seriesGroup);

    auto* selRow = new QHBoxLayout;
    selRow->addWidget(new QLabel(QStringLiteral("选择：")));
    m_seriesCombo = new QComboBox;
    m_seriesCombo->setMinimumWidth(150);
    m_seriesCombo->setMaximumWidth(460);
    selRow->addWidget(m_seriesCombo);
    selRow->addStretch(1);
    sgLayout->addLayout(selRow);

    auto* btnRow = new QHBoxLayout;
    m_colorBtn = new QPushButton(QStringLiteral("颜色"));
    m_colorBtn->setMaximumWidth(60);
    btnRow->addWidget(m_colorBtn);
    m_visibleBtn = new QPushButton(QStringLiteral("显示/隐藏"));
    m_visibleBtn->setMaximumWidth(80);
    btnRow->addWidget(m_visibleBtn);
    m_playBtn = new QPushButton(QStringLiteral("播放选区"));
    m_playBtn->setMaximumWidth(100);
    btnRow->addWidget(m_playBtn);
    m_stopBtn = new QPushButton(QStringLiteral("停止"));
    m_stopBtn->setMaximumWidth(70);
    btnRow->addWidget(m_stopBtn);
    m_deleteBtn = new QPushButton(QStringLiteral("删除曲线"));
    m_deleteBtn->setMaximumWidth(90);
    m_deleteBtn->setStyleSheet("color: red;");
    btnRow->addWidget(m_deleteBtn);
    btnRow->addStretch(1);
    sgLayout->addLayout(btnRow);
    layout->addWidget(seriesGroup, 0, 0);

    // ── Axis controls ─────────────────────────────────────────────────────
    auto* axisGroup = new QGroupBox(QStringLiteral("坐标轴控制"));
    auto* agLayout = new QHBoxLayout(axisGroup);
    agLayout->setContentsMargins(10, 8, 10, 8);
    agLayout->setSpacing(10);

    // X-axis
    auto* xControls = new QWidget;
    auto* xLayout = new QGridLayout(xControls);
    xLayout->setContentsMargins(0, 0, 0, 0);
    xLayout->setHorizontalSpacing(8);
    xLayout->setVerticalSpacing(6);
    xLayout->addWidget(new QLabel(QStringLiteral("X 轴位置")), 0, 0);
    m_xPosSlider = new QSlider(Qt::Horizontal);
    m_xPosSlider->setRange(0, 1000);
    m_xPosSlider->setValue(500);
    m_xPosSlider->setMaximumWidth(480);
    xLayout->addWidget(m_xPosSlider, 0, 1);

    xLayout->addWidget(new QLabel(QStringLiteral("X 轴缩放")), 1, 0);
    m_xZoomSlider = new QSlider(Qt::Horizontal);
    // Store magnification * 100: 100 means 1.00x.
    m_xZoomSlider->setRange(100, 1000);
    m_xZoomSlider->setValue(100);
    m_xZoomSlider->setMaximumWidth(410);
    xLayout->addWidget(m_xZoomSlider, 1, 1);
    m_xZoomEdit = new QLineEdit("1.00x");
    m_xZoomEdit->setFixedWidth(72);
    m_xZoomEdit->setReadOnly(true);
    xLayout->addWidget(m_xZoomEdit, 1, 2);

    m_viewInfoLabel = new QLineEdit;
    m_viewInfoLabel->setReadOnly(true);
    m_viewInfoLabel->setMaximumWidth(550);
    m_viewInfoLabel->setStyleSheet("font-size: 11px;");
    xLayout->addWidget(m_viewInfoLabel, 2, 0, 1, 3);
    xLayout->setColumnStretch(1, 1);
    agLayout->addWidget(xControls, 1, Qt::AlignTop);

    auto* separator = new QFrame;
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    agLayout->addWidget(separator);

    // Y-axis controls follow the axis direction.
    auto* yZoomLayout = new QVBoxLayout;
    auto* yZoomTitle = new QLabel(QStringLiteral("Y 轴缩放"));
    yZoomTitle->setAlignment(Qt::AlignCenter);
    yZoomLayout->addWidget(yZoomTitle);
    m_yZoomSlider = new QSlider(Qt::Vertical);
    m_yZoomSlider->setRange(10, 1000);
    m_yZoomSlider->setValue(100);
    m_yZoomSlider->setMinimumHeight(110);
    m_yZoomSlider->setMaximumHeight(150);
    m_yZoomSlider->setTickPosition(QSlider::TicksRight);
    m_yZoomSlider->setTickInterval(100);
    yZoomLayout->addWidget(m_yZoomSlider, 1, Qt::AlignHCenter);
    m_yZoomEdit = new QLineEdit("1.00x");
    m_yZoomEdit->setFixedWidth(72);
    m_yZoomEdit->setReadOnly(true);
    yZoomLayout->addWidget(m_yZoomEdit, 0, Qt::AlignHCenter);
    agLayout->addLayout(yZoomLayout);

    auto* yPosLayout = new QVBoxLayout;
    auto* yPosTitle = new QLabel(QStringLiteral("Y 轴位置"));
    yPosTitle->setAlignment(Qt::AlignCenter);
    yPosLayout->addWidget(yPosTitle);
    m_yPosSlider = new QSlider(Qt::Vertical);
    m_yPosSlider->setRange(0, 1000);
    m_yPosSlider->setValue(500);
    m_yPosSlider->setMinimumHeight(110);
    m_yPosSlider->setMaximumHeight(150);
    m_yPosSlider->setTickPosition(QSlider::TicksRight);
    m_yPosSlider->setTickInterval(100);
    yPosLayout->addWidget(m_yPosSlider, 1, Qt::AlignHCenter);
    auto* yDirection = new QLabel(QStringLiteral("下 ↕ 上"));
    yDirection->setAlignment(Qt::AlignCenter);
    yPosLayout->addWidget(yDirection);
    agLayout->addLayout(yPosLayout);

    layout->addWidget(axisGroup, 0, 1);

    // ── Frame size ────────────────────────────────────────────────────────
    auto* frameGroup = new QGroupBox(QStringLiteral("全局帧长"));
    auto* fgLayout = new QHBoxLayout(frameGroup);
    fgLayout->setContentsMargins(10, 8, 10, 8);

    auto* scaleLayout = new QVBoxLayout;
    scaleLayout->setSpacing(3);
    auto* maxFrameLabel = new QLabel(QStringLiteral("4096"));
    maxFrameLabel->setAlignment(Qt::AlignCenter);
    scaleLayout->addWidget(maxFrameLabel);

    m_frameSizeSlider = new QSlider(Qt::Vertical);
    m_frameSizeSlider->setRange(6, 12); // 2^6=64 to 2^12=4096 (exponential)
    m_frameSizeSlider->setValue(9);     // 512 (reference program default)
    m_frameSizeSlider->setMinimumHeight(105);
    m_frameSizeSlider->setMaximumHeight(140);
    m_frameSizeSlider->setTickPosition(QSlider::TicksRight);
    m_frameSizeSlider->setTickInterval(1);
    scaleLayout->addWidget(m_frameSizeSlider, 1, Qt::AlignHCenter);

    auto* minFrameLabel = new QLabel(QStringLiteral("64"));
    minFrameLabel->setAlignment(Qt::AlignCenter);
    scaleLayout->addWidget(minFrameLabel);

    auto* valueLayout = new QVBoxLayout;
    valueLayout->addStretch(1);
    auto* currentFrameLabel = new QLabel(QStringLiteral("当前"));
    currentFrameLabel->setAlignment(Qt::AlignCenter);
    valueLayout->addWidget(currentFrameLabel);
    m_frameSizeLabel = new QLabel(QStringLiteral("512"));
    m_frameSizeLabel->setFixedSize(58, 28);
    m_frameSizeLabel->setAlignment(Qt::AlignCenter);
    m_frameSizeLabel->setStyleSheet(QStringLiteral(
        "QLabel { background: palette(window); border: 1px solid palette(mid); "
        "border-radius: 4px; padding: 2px; font-weight: bold; }"));
    valueLayout->addWidget(m_frameSizeLabel, 0, Qt::AlignHCenter);
    valueLayout->addStretch(1);

    fgLayout->addStretch(1);
    fgLayout->addLayout(scaleLayout);
    fgLayout->addSpacing(8);
    fgLayout->addLayout(valueLayout);
    fgLayout->addStretch(1);
    layout->addWidget(frameGroup, 0, 2);

    // ── VAD ───────────────────────────────────────────────────────────────
    auto* vadGroup = new QGroupBox(QStringLiteral("VAD"));
    auto* vgLayout = new QVBoxLayout(vadGroup);

    auto* vadRow = new QHBoxLayout;
    m_vadAlgoCombo = new QComboBox;
    m_vadAlgoCombo->setMaximumWidth(320);
    vadRow->addWidget(m_vadAlgoCombo);
    m_vadRunBtn = new QPushButton(QStringLiteral("执行端点检测"));
    vadRow->addWidget(m_vadRunBtn);
    vadRow->addStretch(1);
    vgLayout->addLayout(vadRow);

    m_vadParamsContainer = new QWidget;
    m_vadParamsContainer->setLayout(new QVBoxLayout);
    vgLayout->addWidget(m_vadParamsContainer);

    layout->addWidget(vadGroup, 0, 3);
    layout->setRowStretch(1, 1);

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

double ControlPanel::xZoom() const { return m_xZoomSlider->value() / 100.0; }
double ControlPanel::xPosition() const { return m_xPosSlider->value() / 1000.0; }
double ControlPanel::yZoom() const { return m_yZoomSlider->value() / 100.0; }
double ControlPanel::yPosition() const { return m_yPosSlider->value() / 1000.0; }

double ControlPanel::xViewMin() const {
    double viewable = m_maxIndex / xZoom();
    double slack = m_maxIndex - viewable;
    double offset = (xPosition() - 0.5) * slack;
    double center = m_maxIndex * 0.5 + offset;
    return std::max(0.0, center - viewable / 2);
}
double ControlPanel::xViewMax() const {
    double viewable = m_maxIndex / xZoom();
    double slack = m_maxIndex - viewable;
    double offset = (xPosition() - 0.5) * slack;
    double center = m_maxIndex * 0.5 + offset;
    return std::min((double)m_maxIndex, center + viewable / 2);
}
double ControlPanel::yViewMin() const {
    double range = m_yAutoMax - m_yAutoMin;
    double viewable = range / yZoom();
    double offset = (yPosition() - 0.5) * 2.0 * range;
    double center = m_yAutoMin + range * 0.5 + offset;
    return center - viewable / 2;
}
double ControlPanel::yViewMax() const {
    double range = m_yAutoMax - m_yAutoMin;
    double viewable = range / yZoom();
    double offset = (yPosition() - 0.5) * 2.0 * range;
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
    // 1.00x shows the full signal. Limit maximum magnification so at least
    // five complete frames remain visible.
    const double maxMagnification = m_maxIndex / (5.0 * frameSize);
    const int maxVal = std::clamp(
        static_cast<int>(std::floor(maxMagnification * 100.0)), 100, 100000);
    m_xZoomSlider->setRange(100, maxVal);
    if (m_xZoomSlider->value() > maxVal)
        m_xZoomSlider->setValue(maxVal);
}
void ControlPanel::setAutoYRange(float yMin, float yMax) {
    m_yAutoMin = yMin; m_yAutoMax = yMax;
    updateViewInfo();
    if (m_chartWidget)
        m_chartWidget->setYRange(m_yAutoMin, m_yAutoMax);
}

// ── Slider updates ──────────────────────────────────────────────────────────

void ControlPanel::onSliderChanged() {
    m_xZoomEdit->setText(
        QStringLiteral("%1x").arg(xZoom(), 0, 'f', 2));
    m_yZoomEdit->setText(
        QStringLiteral("%1x").arg(yZoom(), 0, 'f', 2));
    updateViewInfo();

    // Apply to chart widget
    if (m_chartWidget) {
        m_chartWidget->setXRange(xViewMin(), xViewMax());
        if (!m_chartWidget->selectedKey().isEmpty()) {
            m_chartWidget->setSelectedSeriesYTransform(yZoom(), yPosition());
        } else {
            m_chartWidget->setGlobalYTransform(yZoom(), yPosition());
        }
    }

    m_debounce->start();
}

void ControlPanel::emitViewRangeChanged() { emit viewRangeChanged(); }

void ControlPanel::updateViewInfo() {
    const bool hasSelectedSeries = m_chartWidget
        && !m_chartWidget->selectedKey().isEmpty();
    const QString yInfo = hasSelectedSeries
        ? QStringLiteral("曲线 Y: %1x, 位置 %2%")
              .arg(yZoom(), 0, 'f', 2)
              .arg(yPosition() * 100.0, 0, 'f', 1)
        : QStringLiteral("全局 Y: %1x, 位置 %2%")
              .arg(yZoom(), 0, 'f', 2)
              .arg(yPosition() * 100.0, 0, 'f', 1);
    m_viewInfoLabel->setText(
        QStringLiteral("X: %1-%2  |  %3  |  Total: %4")
            .arg(xViewMin(), 0, 'f', 0).arg(xViewMax(), 0, 'f', 0)
            .arg(yInfo).arg(m_maxIndex, 0, 'f', 0));
}

// ── Series management ───────────────────────────────────────────────────────

void ControlPanel::refreshSeriesSelector() {
    const QString previousKey = m_seriesCombo->currentData().toString();
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
    int index = m_seriesCombo->findData(previousKey);
    if (index < 0) index = 0;
    m_seriesCombo->setCurrentIndex(index);
    m_seriesCombo->blockSignals(false);
    onSeriesSelectorChanged(index);
}

void ControlPanel::onSeriesSelectorChanged(int index) {
    if (index < 0) return;
    QString key = m_seriesCombo->itemData(index).toString();
    emit selectionChanged(key);
    if (m_chartWidget) m_chartWidget->setSelectedKey(key);

    const bool hasSelectedSeries = !key.isEmpty() && m_chartWidget;
    const auto transform = hasSelectedSeries
        ? m_chartWidget->selectedSeriesYTransform()
        : (m_chartWidget
               ? m_chartWidget->globalYTransform()
               : QPair<double, double>{1.0, 0.5});
    {
        const QSignalBlocker zoomBlocker(m_yZoomSlider);
        const QSignalBlocker positionBlocker(m_yPosSlider);
        m_yZoomSlider->setValue(qRound(transform.first * 100.0));
        m_yPosSlider->setValue(qRound(transform.second * 1000.0));
    }
    m_yZoomEdit->setText(
        QStringLiteral("%1x").arg(transform.first, 0, 'f', 2));
    updateViewInfo();
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
        slider->setMaximumWidth(360);
        row->addWidget(slider);

        auto* valLabel = new QLabel;
        valLabel->setFixedWidth(64);
        valLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
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

        row->addStretch(1);
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
