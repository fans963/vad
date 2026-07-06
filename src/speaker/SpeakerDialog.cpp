#include "SpeakerDialog.h"
#include <QMessageBox>

SpeakerDialog::SpeakerDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
}

void SpeakerDialog::setupUi() {
    this->setWindowTitle("Speaker Recognition (GMM/HMM/VQ)");
    this->resize(400, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Speaker Name:", this);
    nameEdit = new QLineEdit(this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    mainLayout->addLayout(nameLayout);

    btnTrain = new QPushButton("Train Model", this);
    btnIdentify = new QPushButton("Identify Speaker", this);
    btnVerify = new QPushButton("Verify Speaker", this);

    connect(btnTrain, &QPushButton::clicked, this, &SpeakerDialog::onTrainModel);
    connect(btnIdentify, &QPushButton::clicked, this, &SpeakerDialog::onIdentifySpeaker);
    connect(btnVerify, &QPushButton::clicked, this, &SpeakerDialog::onVerifySpeaker);

    mainLayout->addWidget(btnTrain);
    mainLayout->addWidget(btnIdentify);
    mainLayout->addWidget(btnVerify);

    resultLabel = new QLabel("Result: ", this);
    mainLayout->addWidget(resultLabel);
    mainLayout->addStretch();
}

void SpeakerDialog::onTrainModel() {
    QMessageBox::information(this, "Info", "Train model triggered.");
}

void SpeakerDialog::onIdentifySpeaker() {
    QMessageBox::information(this, "Info", "Identify speaker triggered.");
}

void SpeakerDialog::onVerifySpeaker() {
    QMessageBox::information(this, "Info", "Verify speaker triggered.");
}
