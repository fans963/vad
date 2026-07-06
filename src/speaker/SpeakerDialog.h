#ifndef SPEAKERDIALOG_H
#define SPEAKERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SpeakerDialog : public QDialog {
    Q_OBJECT
public:
    explicit SpeakerDialog(QWidget *parent = nullptr);

private slots:
    void onTrainModel();
    void onIdentifySpeaker();
    void onVerifySpeaker();

private:
    void setupUi();

    QLineEdit *nameEdit;
    QLabel *resultLabel;
    QPushButton *btnTrain;
    QPushButton *btnIdentify;
    QPushButton *btnVerify;
};

#endif // SPEAKERDIALOG_H
