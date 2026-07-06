#pragma once

#include "Types.h"
#include <QHash>
#include <QString>
#include <optional>

// ── In-memory key-value audio storage ───────────────────────────────────────
class AudioStorage {
public:
    void save(const QString& filePath, AudioData audio);
    std::optional<AudioData> load(const QString& filePath) const;
    void remove(const QString& filePath);
    bool contains(const QString& filePath) const;
    QStringList allKeys() const;

private:
    QHash<QString, AudioData> m_store;
};
