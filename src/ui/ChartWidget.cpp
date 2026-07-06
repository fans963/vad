#include "ChartWidget.h"
#include <QMouseEvent>
#include <QMenu>

ChartWidget::ChartWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_plot = new QCustomPlot(this);
    m_plot->setInteractions({});
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
    });

    layout->addWidget(m_plot);
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
            m_meta[k] = { c, true };
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
    m_plot->yAxis->setRange(min, max);
    m_plot->replot(QCustomPlot::rpQueuedReplot);
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
    m_plot->clearGraphs();

    for (const auto& key : allSeriesKeys()) {
        if (!isVisibleByKey(key)) continue;

        const auto& chart = m_data[key];
        const auto& meta = m_meta[key];
        auto [filePath, dt] = parseSeriesKey(key);

        bool selected = (key == m_selectedKey);
        double lineWidth = selected ? 1.5 : 0.4;
        double opacity = selected ? 1.0 : 0.7;

        auto* graph = m_plot->addGraph();
        graph->setAdaptiveSampling(true); // 自动降采样：跳过屏幕重叠点
        QString label = QStringLiteral("%1 %2").arg(
            filePath.section(QLatin1Char('/'), -1), dataTypeDisplayName(dt));
        graph->setName(label);

        // Copy data
        QVector<double> xs(chart.points.size()), ys(chart.points.size());
        for (int i = 0; i < chart.points.size(); ++i) {
            xs[i] = chart.points[i].x;
            ys[i] = chart.points[i].y;
        }

        switch (dt) {
        case DataType::Energy:
        case DataType::ZeroCrossingRate: {
            // StepLineSeries in Flutter — use QCPStepLine in QCustomPlot
            graph->setLineStyle(QCPGraph::lsStepLeft);
            QColor c = meta.color;
            c.setAlphaF(opacity);
            QPen pen(c);
            pen.setWidthF(lineWidth);
            graph->setPen(pen);
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

bool ChartWidget::isVisibleByKey(const QString& key) const {
    auto it = m_meta.find(key);
    return it == m_meta.end() || it->visible;
}

QString ChartWidget::selectedKey() const { return m_selectedKey; }

void ChartWidget::setSelectedKey(const QString& key) {
    m_selectedKey = key;
    rebuildSeries();
}

int ChartWidget::plotWidth() const { return m_plot->viewport().width(); }

void ChartWidget::setFrameGrid(int frameSize) {
    m_frameGridSize = frameSize;
    m_plot->xAxis->grid()->setVisible(true);
    m_plot->xAxis->grid()->setPen(QPen(QColor(128, 128, 128, 60), 0.5));
    m_plot->xAxis->setSubTicks(false);
    applyFrameGrid();
}

void ChartWidget::applyFrameGrid() {
    if (m_frameGridSize <= 0) return;
    double w = m_plot->xAxis->range().size();
    if (w <= 0) return;
    // step ≥ frameSize, adaptive to keep ≤15 lines
    int n = std::max(1, (int)std::ceil(w / (15.0 * m_frameGridSize)));
    double step = n * m_frameGridSize;
    auto* t = new QCPAxisTickerFixed;
    t->setTickStep(step);
    t->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_plot->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(t));
    m_plot->replot();
}

QStringList ChartWidget::allSeriesKeys() const {
    return m_data.keys();
}
