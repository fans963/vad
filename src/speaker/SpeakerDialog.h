#pragma once
#include <QDialog>
#include <QVector>

class QComboBox; class QLabel; class QLineEdit; class QListWidget; class QSlider;

class SpeakerDialog : public QDialog {
    Q_OBJECT
public:
    explicit SpeakerDialog(QWidget* parent = nullptr);
private slots:
    void chooseTrainingFiles(); void chooseTestFile(); void trainModel();
    void identify(); void verify(); void removeSpeaker();
private:
    struct Component { double weight{}; QVector<double> mean, variance; };
    struct Model { QString name; QVector<Component> components; };
    void setupUi();
    QVector<QVector<double>> features(const QStringList& files) const;
    Model train(const QString& name, const QVector<QVector<double>>& data, int order) const;
    double score(const Model& model, const QVector<QVector<double>>& data) const;
    QString modelPath() const; void loadModels(); bool saveModels() const; void refresh();
    QListWidget* m_speakers{}; QLineEdit* m_name{}; QLineEdit* m_trainFiles{};
    QLineEdit* m_testFile{}; QComboBox* m_order{}; QSlider* m_threshold{};
    QLabel* m_thresholdText{}; QLabel* m_result{};
    QStringList m_trainingPaths; QString m_testPath; QVector<Model> m_models;
};
