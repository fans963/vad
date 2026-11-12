#pragma once

#include <QWidget>
#include <QListWidget>
#include <QHash>
#include "../../core/Types.h"

class InfoPanel : public QWidget {
    Q_OBJECT
public:
    explicit InfoPanel(QWidget* parent = nullptr);
    void setAudioInfoMap(const QHash<QString, AudioInfo>& infoMap);
    void setSelectedFile(const QString& filePath);

signals:
    void fileSelected(const QString& filePath);

private:
    QListWidget* m_list = nullptr;
};
