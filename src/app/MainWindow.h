#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>

#include "../core/AudioEngine.h"
#include "../ui/ChartWidget.h"
#include "../ui/panels/ControlPanel.h"
#include "../ui/panels/InfoPanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void loadAudioFile(const QString& filePath, const QByteArray& data = {});
    QString currentFilePath() const;
    ChartWidget* chartWidget() const { return m_chartWidget; }
    ControlPanel* controlPanel() const { return m_controlPanel; }
    InfoPanel* infoPanel() const { return m_infoPanel; }
    bool isMenuActionChecked(DataType dt) const {
        return m_curveActions.contains(dt) && m_curveActions[dt]->isChecked();
    }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onOpenFile();
    void saveCurrentSelection();
    void exportChart();
    void exportEffectiveMfcc();
    void onChartEvent(const ChartEvent& event);
    void onViewRangeChanged();
    void onGlobalFrameSizeChanged(int newSize);
    void onPlayAudio(const QString& filePath, double startFraction);
    void onStopAudio();
    void onVadAlgorithmChanged(const QString& name);
    void onVadParamChanged(const QString& key, float value);
    void onRunVad(const QString& filePath);
    void onVadRunForCurrentFile();

public slots:
    void setCurrentFile(const QString& filePath);
    void onMenuCurveToggled(DataType dt, bool checked);

private:
    void setupUi();
    void setupMenus();
    void wireSignals();
    void initState();
    void addCurveForSelected(DataType dt);
    void toggleCurveForSelected(DataType dt, bool visible);
    void refreshFileSelector();
    void updateMenuActionStates();

    AudioEngine* m_engine = nullptr;
    ChartWidget* m_chartWidget = nullptr;
    ControlPanel* m_controlPanel = nullptr;
    InfoPanel* m_infoPanel = nullptr;
    QStackedWidget* m_panelStack = nullptr;
    QComboBox* m_fileSelector = nullptr;
    QTimer* m_vadRefreshTimer = nullptr;
    QString m_pendingVadFile;
    QString m_currentFilePath;
    QHash<DataType, QAction*> m_curveActions;

    // Audio info tracking
    QHash<QString, AudioInfo> m_audioInfoMap;
};
