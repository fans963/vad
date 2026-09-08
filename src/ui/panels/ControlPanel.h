#pragma once

#include <QWidget>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include "../../core/Types.h"

class ChartWidget;

// ── Control panel: axis controls, series management, VAD, playback ────────
class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(QWidget* parent = nullptr);
    void setChartWidget(ChartWidget* cw);

    // View state accessors
    double xZoom() const;
    double xPosition() const;
    double yZoom() const;
    double yPosition() const;
    double xViewMin() const;
    double xViewMax() const;
    double yViewMin() const;
    double yViewMax() const;

    int globalFrameSize() const;
    void setGlobalFrameSize(int size);

    void setChartMaxIndex(float idx);
    void setAutoYRange(float yMin, float yMax);

signals:
    void viewRangeChanged();
    void globalFrameSizeChanged(int newSize);
    void playAudioRequested(const QString& filePath, double startFraction);
    void pauseRequested();
    void resumeRequested();
    void stopRequested();
    void seekRequested(double fraction);
    void speedChanged(float multiplier);
    void deleteSeriesRequested(const QString& filePath, DataType dt);
    void visibilityToggled(const QString& filePath, DataType dt, bool visible);
    void colorChanged(const QString& filePath, DataType dt, const QColor& color);
    void selectionChanged(const QString& seriesKey);
    void vadRunRequested(const QString& filePath);
    void vadAlgorithmChanged(const QString& name);
    void vadParamChanged(const QString& key, float value);

public slots:
    void refreshSeriesSelector();
    void setVadAlgorithms(const QStringList& names, const QString& current);
    void setVadParams(const QVector<VadParamDef>& params);
    void setPlaybackState(const PlaybackState& state, float chartPosition);

private slots:
    void onSliderChanged();
    void onSeriesSelectorChanged(int index);
    void onShowColorPicker();
    void onToggleVisibility();
    void onDeleteSelected();
    void onPlaySelected();
    void onStop();
    void onVadAlgorithmChanged(int index);
    void onVadParamChanged();
    void onRunVad();

private:
    void setupUi();
    void updateViewInfo();
    void updateXZoomRange();
    void emitViewRangeChanged();

    ChartWidget* m_chartWidget = nullptr;
    float m_maxIndex = 10000.0f;
    float m_yAutoMin = -0.5f;
    float m_yAutoMax = 0.5f;

    // Sliders
    QSlider* m_xZoomSlider = nullptr;
    QSlider* m_xPosSlider = nullptr;
    QSlider* m_yZoomSlider = nullptr;
    QSlider* m_yPosSlider = nullptr;
    QSlider* m_frameSizeSlider = nullptr;
    QLabel* m_frameSizeLabel = nullptr;

    QLineEdit* m_xZoomEdit = nullptr;
    QLineEdit* m_yZoomEdit = nullptr;
    QLineEdit* m_viewInfoLabel = nullptr;

    // Series selector
    QComboBox* m_seriesCombo = nullptr;
    QPushButton* m_colorBtn = nullptr;
    QPushButton* m_visibleBtn = nullptr;
    QPushButton* m_deleteBtn = nullptr;
    QPushButton* m_playBtn = nullptr;
    QPushButton* m_stopBtn = nullptr;

    // VAD
    QComboBox* m_vadAlgoCombo = nullptr;
    QPushButton* m_vadRunBtn = nullptr;
    QWidget* m_vadParamsContainer = nullptr;
    QVector<QPair<QString, QSlider*>> m_vadParamSliders;

    QTimer* m_debounce = nullptr;
};
