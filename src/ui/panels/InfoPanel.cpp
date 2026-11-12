#include "InfoPanel.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSignalBlocker>

InfoPanel::InfoPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(QStringLiteral("音频文件信息"));
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);

    m_list = new QListWidget;
    m_list->setAlternatingRowColors(true);
    layout->addWidget(m_list);

    connect(m_list, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        if (item) {
            QString fp = item->data(Qt::UserRole).toString();
            if (!fp.isEmpty()) emit fileSelected(fp);
        }
    });

    auto* hint = new QLabel(QStringLiteral("尚未加载音频，可拖放或点击“打开”。"));
    hint->setStyleSheet("color: #888;");
    hint->setAlignment(Qt::AlignCenter);
    layout->addWidget(hint);
}

void InfoPanel::setAudioInfoMap(const QHash<QString, AudioInfo>& infoMap) {
    const QString selected = m_list->currentItem()
        ? m_list->currentItem()->data(Qt::UserRole).toString() : QString();
    m_list->clear();
    int selectIdx = -1;
    for (auto it = infoMap.begin(); it != infoMap.end(); ++it) {
        const auto& info = it.value();
        QString text = QStringLiteral("%1\n  采样率: %2 Hz  |  声道: %3  |  时长: %4s  |  格式: %5")
                           .arg(info.filePath.split('/').last())
                           .arg(info.sampleRate)
                           .arg(info.channels)
                           .arg(info.durationSecs, 0, 'f', 2)
                           .arg(info.format);
        auto* item = new QListWidgetItem(text, m_list);
        item->setData(Qt::UserRole, info.filePath);
        if (info.filePath == selected)
            selectIdx = m_list->count() - 1;
    }
    if (selectIdx >= 0) {
        const QSignalBlocker blocker(m_list);
        m_list->setCurrentRow(selectIdx);
    }
}

void InfoPanel::setSelectedFile(const QString& filePath) {
    for (int i = 0; i < m_list->count(); ++i) {
        if (m_list->item(i)->data(Qt::UserRole).toString() == filePath) {
            const QSignalBlocker blocker(m_list);
            m_list->setCurrentRow(i);
            break;
        }
    }
}
