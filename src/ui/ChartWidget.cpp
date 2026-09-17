#include "ChartWidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QPalette>
#include <algorithm>

namespace {

class FrameAxisTicker final : public QCPAxisTickerFixed {
protected:
    int getSubTickCount(double /*tickStep*/) override { return 1; }
};

} // namespace

ChartWidget::ChartWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_plot = new QCustomPlot(this);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot->setPlottingHint(QCP::phFastPolylines, true); // 快速折线
    m_plot->xAxis->setLabel(QStringLiteral("Index"));
    m_plot->yAxis->setLabel(QStringLiteral("Amplitude"));
    m_plot->legend->setVisible(true);
    m_plot->legend->setFont(QFont(m_plot->font().family(), 9));
    m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    // X axis: no grid lines (like Flutter)
    m_plot->xAxis->grid()->setVisible(false);
    m_plot->yAxis->grid()->setVisible(false);

    // Coordinate label
    m_coordLabel = new QCPItemText(m_plot);
    m_coordLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_coordLabel->position->setCoords(0.02, 0.02);
    m_coordLabel->setText(QString());
    m_coordLabel->setPen(QPen(Qt::black));
    m_coordLabel->setBrush(QBrush(QColor(255, 255, 255, 180)));

    // Playhead line (like Flutter PlotBand)
    m_playhead = new QCPItemLine(m_plot);
    m_playhead->start->setCoords(0, 0);
    m_playhead->end->setCoords(0, 1);
    m_playhead->setPen(QPen(Qt::red, 1.5));
    m_playhead->setVisible(false);

    auto setupMarker = [this](QCPItemLine*& line) {
        line = new QCPItemLine(m_plot);
        line->start->setTypeY(QCPItemPosition::ptAxisRectRatio);
        line->end->setTypeY(QCPItemPosition::ptAxisRectRatio);
        line->start->setCoords(0, 0);
        line->end->setCoords(0, 1);
        line->setPen(QPen(QColor(255, 140, 0), 1.5, Qt::DashLine));
        line->setVisible(false);
    };
    setupMarker(m_selectionStartLine);
    setupMarker(m_selectionEndLine);
    applyPlotPalette();

    // Resize tracking for adaptive downsampling
    connect(m_plot, &QCustomPlot::afterReplot, this, [this]() {
        int w = m_plot->viewport().width();
        if (w != m_lastWidth) {
            m_lastWidth = w;
            emit plotWidthChanged(w);
        }
    });

    // Mouse tracking for coordinate display
    connect(m_plot, &QCustomPlot::mouseMove, this, [this](QMouseEvent* e) {
        double x = m_plot->xAxis->pixelToCoord(e->pos().x());
        double y = m_plot->yAxis->pixelToCoord(e->pos().y());
        m_coordLabel->setText(QStringLiteral("x:%1 y:%2").arg(x, 0, 'f', 0).arg(y, 0, 'f', 4));
        m_plot->replot(QCustomPlot::rpQueuedReplot);
        if (m_selecting) {
            m_selectionEnd = std::max(0.0, x);
            m_selectionEndLine->start->setCoords(m_selectionEnd, 0);
            m_selectionEndLine->end->setCoords(m_selectionEnd, 1);
            m_selectionEndLine->setVisible(true);
        }
    });
    connect(m_plot, &QCustomPlot::mousePress, this, [this](QMouseEvent* e) {
        if (e->button() != Qt::LeftButton || !(e->modifiers() & Qt::ShiftModifier))
            return;
        m_selecting = true;
        m_selectionStart = std::max(0.0, m_plot->xAxis->pixelToCoord(e->pos().x()));
        m_selectionEnd = m_selectionStart;
        for (auto* line : {m_selectionStartLine, m_selectionEndLine}) {
            line->start->setCoords(m_selectionStart, 0);
            line->end->setCoords(m_selectionStart, 1);
            line->setVisible(true);
        }
    });
    connect(m_plot, &QCustomPlot::mouseRelease, this, [this](QMouseEvent*) {
        if (!m_selecting) return;
        m_selecting = false;
        if (m_selectionStart > m_selectionEnd)
            std::swap(m_selectionStart, m_selectionEnd);
        emit selectionRangeChanged(m_selectionStart, m_selectionEnd);
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    });

    layout->addWidget(m_plot);
}

void ChartWidget::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::PaletteChange ||
        event->type() == QEvent::ApplicationPaletteChange) {
        applyPlotPalette();
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void ChartWidget::applyPlotPalette() {
    if (!m_plot) return;

    const QPalette& pal = palette();
    const QColor background = pal.color(QPalette::Base);
    const QColor text = pal.color(QPalette::Text);
    const QColor border = pal.color(QPalette::Mid);
    const bool dark = background.lightness() < 128;

    m_plot->setPalette(pal);
    m_plot->setBackground(QBrush(background));
    m_plot->axisRect()->setBackground(QBrush(background));

    QColor axisColor = text;
    axisColor.setAlpha(dark ? 210 : 190);
    QColor subTickColor = text;
    subTickColor.setAlpha(dark ? 130 : 110);
    for (auto* axis : {m_plot->xAxis, m_plot->xAxis2,
                       m_plot->yAxis, m_plot->yAxis2}) {
        axis->setBasePen(QPen(axisColor, 1.0));
        axis->setTickPen(QPen(axisColor, 1.0));
        axis->setSubTickPen(QPen(subTickColor, 1.0));
        axis->setTickLabelColor(text);
        axis->setLabelColor(text);
    }

    QColor legendBackground = background;
    legendBackground.setAlpha(dark ? 225 : 235);
    m_plot->legend->setBrush(QBrush(legendBackground));
    m_plot->legend->setBorderPen(QPen(border));
    m_plot->legend->setTextColor(text);
    m_plot->legend->setSelectedTextColor(text);

    if (m_coordLabel) {
        QColor labelBackground = background;
        labelBackground.setAlpha(dark ? 225 : 220);
        m_coordLabel->setColor(text);
        m_coordLabel->setPen(QPen(border));
        m_coordLabel->setBrush(QBrush(labelBackground));
    }

    if (m_playhead)
        m_playhead->setPen(QPen(dark ? QColor(255, 92, 92)
                                     : QColor(220, 30, 30), 1.5));
    const QColor selectionColor = dark ? QColor(255, 183, 77)
                                       : QColor(230, 125, 0);
    for (auto* line : {m_selectionStartLine, m_selectionEndLine}) {
        if (line) line->setPen(QPen(selectionColor, 1.5, Qt::DashLine));
    }

    updateFrameGridStyle();
}

void ChartWidget::updateFrameGridStyle() {
    if (!m_plot) return;
    const bool dark = palette().color(QPalette::Base).lightness() < 128;
    auto* grid = m_plot->xAxis->grid();

    // Keep full-frame and half-frame boundaries distinct on both themes.
    const QColor fullFrame = dark ? QColor(105, 170, 255, 155)
                                  : QColor(48, 105, 180, 130);
    const QColor halfFrame = dark ? QColor(255, 190, 90, 145)
                                  : QColor(230, 145, 35, 115);
    grid->setPen(QPen(fullFrame, 1.0, Qt::SolidLine));
    grid->setSubGridPen(QPen(halfFrame, 0.8, Qt::DashLine));
}

void ChartWidget::handleChartEvent(const ChartEvent& event) {
    switch (event.type) {
    case ChartEvent::AddChart: {
        QString k = makeSeriesKey(event.key, event.dataType);
        m_data[k] = event.chart;
        if (!m_meta.contains(k)) {
            QColor c = (event.dataType == DataType::Vad)
                ? QColor(76, 175, 80) // green for VAD
                : kPalette[m_nextColorIndex++ % kPaletteSize];
            m_meta[k] = { c, event.chart.visible };
        } else {
            // The engine owns visibility state. Keep the user's color, but
            // always accept a later hidden -> visible transition.
            m_meta[k].visible = event.chart.visible;
        }
        rebuildSeries();
        // Auto-rescale on first chart
        if (m_data.size() <= 4) {
            m_plot->rescaleAxes();
            m_plot->replot();
        }
        break;
    }
    case ChartEvent::UpdateAllCharts: {
        m_data.clear();
        for (const auto& [key, chart] : event.chartList) {
            QString k = makeSeriesKey(key, chart.dataType);
            m_data[k] = chart;
            if (!m_meta.contains(k)) {
                QColor c = (chart.dataType == DataType::Vad)
                    ? QColor(76, 175, 80)
                    : kPalette[m_nextColorIndex++ % kPaletteSize];
                m_meta[k] = { c, chart.visible };
            } else {
                m_meta[k].visible = chart.visible;
            }
        }
        rebuildSeries();
        break;
    }
    case ChartEvent::RemoveChart: {
        m_data.remove(makeSeriesKey(event.key, event.dataType));
        m_meta.remove(makeSeriesKey(event.key, event.dataType));
        rebuildSeries();
        break;
    }
    case ChartEvent::RemoveAllCharts:
        m_data.clear();
        m_meta.clear();
        rebuildSeries();
        break;
    case ChartEvent::UpdateYRange:
    case ChartEvent::UpdateMaxIndex:
        break;
    case ChartEvent::UpdatePlaybackState:
        setPlayheadPosition(event.chartPosition);
        break;
    }
}

void ChartWidget::setXRange(double min, double max) {
    m_plot->xAxis->setRange(min, max);
    applyFrameGrid();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void ChartWidget::setYRange(double min, double max) {
    if (max <= min) return;
    m_autoYMin = min;
    m_autoYMax = max;
    applyGlobalYRange();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void ChartWidget::applyGlobalYRange() {
    const double baseMin = m_stackedSeries ? 0.0 : m_autoYMin;
    const double baseMax = m_stackedSeries
        ? std::max(1, m_stackedLaneCount)
        : m_autoYMax;
    const double baseRange = std::max(1e-12, baseMax - baseMin);
    const double viewRange = baseRange / m_globalYScale;
    const double baseCenter = 0.5 * (baseMin + baseMax);
    // Position 50% keeps the automatic range centred. Moving towards either
    // end translates the viewport by up to one complete automatic range.
    const double offset = (m_globalYPosition - 0.5) * 2.0 * baseRange;
    const double center = baseCenter + offset;
    m_plot->yAxis->setRange(center - viewRange * 0.5,
                            center + viewRange * 0.5);
}

void ChartWidget::setPlayheadPosition(float x) {
    if (x >= 0) {
        m_playhead->start->setCoords(x, 0);
        m_playhead->end->setCoords(x, 1);
        m_playhead->setVisible(true);
    } else {
        m_playhead->setVisible(false);
    }
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

// ── Rebuild all graph series (matching Flutter chart_widget.dart styles) ──

void ChartWidget::rebuildSeries() {
    m_plot->clearPlottables();
    m_plot->yAxis2->setVisible(false);
    int lineSeriesCount = 0;
    for (const auto& key : allSeriesKeys())
        if (isVisibleByKey(key) && m_data[key].dataType != DataType::Spectrogram)
            ++lineSeriesCount;
    m_stackedSeries = lineSeriesCount > 1;
    m_stackedLaneCount = lineSeriesCount;
    int lane = 0;

    for (const auto& key : allSeriesKeys()) {
        if (!isVisibleByKey(key)) continue;

        const auto& chart = m_data[key];
        const auto& meta = m_meta[key];
        auto [filePath, dt] = parseSeriesKey(key);

        if (dt == DataType::Spectrogram && chart.heatColumns > 0 && chart.heatRows > 0) {
            auto* map = new QCPColorMap(m_plot->xAxis, m_plot->yAxis2);
            m_plot->yAxis2->setVisible(true);
            m_plot->yAxis2->setLabel(QStringLiteral("频率 (Hz)"));
            m_plot->yAxis2->setRange(0, std::max(1.0f, chart.heatYMax));
            map->setName(QStringLiteral("%1 %2").arg(
                filePath.section(QLatin1Char('/'), -1), dataTypeDisplayName(dt)));
            map->data()->setSize(chart.heatColumns, chart.heatRows);
            map->data()->setRange(QCPRange(0, std::max(1.0f, chart.heatXMax)),
                                  QCPRange(0, std::max(1.0f, chart.heatYMax)));
            for (int x = 0; x < chart.heatColumns; ++x)
                for (int y = 0; y < chart.heatRows; ++y)
                    map->data()->setCell(x, y, chart.heatValues[x * chart.heatRows + y]);
            map->setGradient(QCPColorGradient::gpJet);
            map->setDataRange(QCPRange(chart.minY, chart.maxY));
            map->setInterpolate(false);
            continue;
        }

        bool selected = (key == m_selectedKey);
        // Keep every series fully saturated. Selection is indicated by line
        // width only, so unselected curves don't look disabled or washed out.
        double lineWidth = selected ? 2.0 : 1.0;
        double opacity = 1.0;

        auto* graph = m_plot->addGraph();
        graph->setAdaptiveSampling(true); // 自动降采样：跳过屏幕重叠点
        QString label = QStringLiteral("%1 %2").arg(
            filePath.section(QLatin1Char('/'), -1), dataTypeDisplayName(dt));
        graph->setName(label);

        // Copy data
        QVector<double> xs(chart.points.size()), ys(chart.points.size());
        const double rawChartRange = double(chart.maxY - chart.minY);
        const double chartRange = std::max(1e-12, rawChartRange);
        const double chartCenter = 0.5 * double(chart.minY + chart.maxY);
        for (int i = 0; i < chart.points.size(); ++i) {
            xs[i] = chart.points[i].x;
            if (m_stackedSeries) {
                const double normalized = std::abs(rawChartRange) > 1e-12
                    ? (chart.points[i].y - chart.minY) / chartRange
                    : 0.5;
                const double transformed = 0.5
                    + (normalized - 0.5) * meta.yScale
                    + (meta.yPosition - 0.5) * 2.0;
                ys[i] = lane + 0.1 + 0.8 * transformed;
            } else {
                const double offset = (meta.yPosition - 0.5) * 2.0 * chartRange;
                ys[i] = chartCenter
                    + (chart.points[i].y - chartCenter) * meta.yScale
                    + offset;
            }
        }
        ++lane;

        switch (dt) {
        case DataType::Energy:
        case DataType::ZeroCrossingRate: {
            // Connect the value at each frame center to make trends easy to see.
            graph->setLineStyle(QCPGraph::lsLine);
            QColor c = meta.color;
            c.setAlphaF(opacity);
            QPen pen(c);
            pen.setWidthF(lineWidth);
            graph->setPen(pen);
            graph->setScatterStyle(QCPScatterStyle::ssNone);
            graph->setData(xs, ys);
            break;
        }
        case DataType::Vad: {
            // AreaSeries in Flutter — green fill with opacity
            graph->setLineStyle(QCPGraph::lsLine);
            QPen borderPen(QColor(76, 175, 80, static_cast<int>(255 * 0.6)));
            borderPen.setWidthF(1.0);
            graph->setPen(borderPen);
            graph->setBrush(QColor(76, 175, 80, static_cast<int>(255 * 0.25)));
            graph->setData(xs, ys);
            break;
        }
        default: {
            // Audio / Spectrum — FastLineSeries in Flutter
            graph->setLineStyle(QCPGraph::lsLine);
            QColor c = meta.color;
            c.setAlphaF(opacity);
            QPen pen(c);
            pen.setWidthF(lineWidth);
            graph->setPen(pen);

            // Don't use scatter points for performance with large datasets
            graph->setScatterStyle(QCPScatterStyle::ssNone);
            graph->setData(xs, ys);
            break;
        }
        }
    }

    if (m_stackedSeries) {
        m_plot->yAxis->setLabel(QStringLiteral("特征曲线（独立归一化分层）"));
    } else {
        m_plot->yAxis->setLabel(QStringLiteral("幅度 / 特征值"));
    }

    applyGlobalYRange();
    m_plot->replot();
}

QColor ChartWidget::seriesColor(const QString& filePath, DataType dt) const {
    auto it = m_meta.find(makeSeriesKey(filePath, dt));
    if (it != m_meta.end()) return it->color;
    return kPalette[0];
}

bool ChartWidget::isVisible(const QString& filePath, DataType dt) const {
    auto it = m_meta.find(makeSeriesKey(filePath, dt));
    return it == m_meta.end() || it->visible;
}

void ChartWidget::setSeriesVisible(const QString& filePath, DataType dt, bool visible) {
    m_meta[makeSeriesKey(filePath, dt)].visible = visible;
    rebuildSeries();
}

void ChartWidget::setSeriesColor(const QString& filePath, DataType dt, const QColor& color) {
    m_meta[makeSeriesKey(filePath, dt)].color = color;
    rebuildSeries();
}

QPair<double, double> ChartWidget::selectionRange() const {
    return {m_selectionStart, m_selectionEnd};
}

void ChartWidget::clearSelection() {
    m_selectionStart = m_selectionEnd = -1.0;
    m_selectionStartLine->setVisible(false);
    m_selectionEndLine->setVisible(false);
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

bool ChartWidget::savePng(const QString& path) const {
    return m_plot->savePng(path, 0, 0, 1.5, 95);
}

bool ChartWidget::savePdf(const QString& path) const {
    return m_plot->savePdf(path);
}

bool ChartWidget::isVisibleByKey(const QString& key) const {
    auto it = m_meta.find(key);
    return it == m_meta.end() || it->visible;
}

QString ChartWidget::selectedKey() const { return m_selectedKey; }

void ChartWidget::setSelectedKey(const QString& key) {
    m_selectedKey = key;
    rebuildSeries();
}

void ChartWidget::setSelectedSeriesYTransform(double scale, double position) {
    auto it = m_meta.find(m_selectedKey);
    if (it == m_meta.end()) return;
    it->yScale = std::clamp(scale, 0.1, 10.0);
    it->yPosition = std::clamp(position, 0.0, 1.0);
    rebuildSeries();
}

QPair<double, double> ChartWidget::selectedSeriesYTransform() const {
    auto it = m_meta.constFind(m_selectedKey);
    if (it == m_meta.cend()) return {1.0, 0.5};
    return {it->yScale, it->yPosition};
}

void ChartWidget::setGlobalYTransform(double scale, double position) {
    m_globalYScale = std::clamp(scale, 0.1, 10.0);
    m_globalYPosition = std::clamp(position, 0.0, 1.0);
    applyGlobalYRange();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

QPair<double, double> ChartWidget::globalYTransform() const {
    return {m_globalYScale, m_globalYPosition};
}

int ChartWidget::plotWidth() const { return m_plot->viewport().width(); }

void ChartWidget::setFrameGrid(int frameSize) {
    m_frameGridSize = frameSize;
    auto* grid = m_plot->xAxis->grid();
    const bool enabled = frameSize > 0;
    grid->setVisible(enabled);
    grid->setSubGridVisible(enabled);

    updateFrameGridStyle();
    m_plot->xAxis->setSubTicks(enabled);
    applyFrameGrid();
}

void ChartWidget::applyFrameGrid() {
    if (m_frameGridSize <= 0) {
        m_plot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    auto* t = new FrameAxisTicker;
    t->setTickStep(m_frameGridSize);
    t->setTickOrigin(0.0);
    t->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(t));
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

QStringList ChartWidget::allSeriesKeys() const {
    QStringList keys = m_data.keys();
    keys.sort(Qt::CaseInsensitive);
    return keys;
}
