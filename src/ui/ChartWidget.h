#pragma once

#include <QWidget>
#include <qcustomplot.h>
#include "../core/Types.h"
#include <QHash>
#include <QVector>

// ── Chart widget ────────────────────────────────────────────────────────────
// Displays audio waveforms, spectra, energy, ZCR, and VAD curves using QCustomPlot.
class ChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChartWidget(QWidget* parent = nullptr);

    void handleChartEvent(const ChartEvent& event);

    // Viewport control
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    void setPlayheadPosition(float x);

    // Series management (public for ControlPanel)
    QStringList allSeriesKeys() const;
    void setSelectedKey(const QString& key);
    QString selectedKey() const;
    bool isVisible(const QString& filePath, DataType dt) const;
    void setSeriesVisible(const QString& filePath, DataType dt, bool visible);
    void setSeriesColor(const QString& filePath, DataType dt, const QColor& color);
    QPair<double, double> selectionRange() const;
    void clearSelection();
    bool savePng(const QString& path) const;
    bool savePdf(const QString& path) const;

    int plotWidth() const;
    void setFrameGrid(int frameSize);
    void applyFrameGrid();

signals:
    void xRangeChanged(double min, double max);
    void plotWidthChanged(int pixels);
    void selectionRangeChanged(double firstSample, double lastSample);

public:
    void rebuildSeries();

private:
    QColor seriesColor(const QString& filePath, DataType dt) const;
    bool isVisibleByKey(const QString& key) const;

    QCustomPlot* m_plot = nullptr;
    QCPItemText* m_coordLabel = nullptr;
    QCPItemLine* m_playhead = nullptr;
    QCPItemLine* m_selectionStartLine = nullptr;
    QCPItemLine* m_selectionEndLine = nullptr;
    double m_selectionStart = -1.0;
    double m_selectionEnd = -1.0;
    bool m_selecting = false;
    int m_lastWidth = 0;
    int m_frameGridSize = 0;

    // Chart data: key ("/path|datatype") -> CachedChart
    QHash<QString, CachedChart> m_data;

    // Series metadata: color, visibility
    struct Meta {
        QColor color;
        bool visible = true;
    };
    QHash<QString, Meta> m_meta;
    QString m_selectedKey;
    int m_nextColorIndex = 0;
    bool m_stackedSeries = false;

    // Default color palette
    static constexpr int kPaletteSize = 8;
    const QColor kPalette[kPaletteSize] = {
        QColor(66, 133, 244),  // blue
        QColor(234, 67, 53),   // red
        QColor(52, 168, 83),   // green
        QColor(251, 188, 4),   // yellow/orange
        QColor(155, 82, 222),  // purple
        QColor(0, 188, 212),   // cyan
        QColor(233, 30, 99),   // pink
        QColor(255, 193, 7),   // amber
    };
};
