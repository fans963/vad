#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>

#include "../core/AudioEngine.h"
#include "../ui/ChartWidget.h"
#include "../ui/panels/ControlPanel.h"
#include "../ui/panels/InfoPanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onOpenFile();
    void openIntoSlot(int slot);
    void saveSlot(int slot);
    void linkFirstTwoFiles();
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

private:
    void setupUi();
    void setupMenus();
    void wireSignals();
    void initState();
    void loadAudioFile(const QString& filePath, const QByteArray& data = {});
    QString currentFilePath() const;
    void addCurveForSelected(DataType dt);
    void toggleCurveForSelected(DataType dt, bool visible);
    void refreshFileSelector();

    AudioEngine* m_engine = nullptr;
    ChartWidget* m_chartWidget = nullptr;
    ControlPanel* m_controlPanel = nullptr;
    InfoPanel* m_infoPanel = nullptr;
    QStackedWidget* m_panelStack = nullptr;
    QComboBox* m_fileSelector = nullptr;

    // Audio info tracking
    QHash<QString, AudioInfo> m_audioInfoMap;
    QString m_fileSlots[3];
};
