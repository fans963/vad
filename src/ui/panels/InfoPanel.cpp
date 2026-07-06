#include "InfoPanel.h"
#include <QVBoxLayout>
#include <QLabel>

InfoPanel::InfoPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(QStringLiteral("Audio File Info"));
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    m_list = new QListWidget;
    m_list->setAlternatingRowColors(true);
    layout->addWidget(m_list);

    auto* hint = new QLabel(QStringLiteral("No audio files loaded. Drag & drop or click + to add."));
    hint->setStyleSheet("color: #888;");
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);
}

void InfoPanel::setAudioInfoMap(const QHash<QString, AudioInfo>& infoMap) {
    m_list->clear();
    for (auto it = infoMap.begin(); it != infoMap.end(); ++it) {
        const auto& info = it.value();
        QString text = QStringLiteral("%1\n  SR: %2 Hz  |  Ch: %3  |  Duration: %4s  |  Format: %5")
                           .arg(info.filePath.split('/').last())
                           .arg(info.sampleRate)
                           .arg(info.channels)
                           .arg(info.durationSecs, 0, 'f', 2)
                           .arg(info.format);
        m_list->addItem(text);
    }
}
