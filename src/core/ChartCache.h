#pragma once

#include "Types.h"
#include <QHash>
#include <QVector>
#include <optional>
#include <mutex>

// ── Thread-safe in-memory cached chart storage ──────────────────────────────
class ChartCache {
public:
    void add(const QString& filePath, const CachedChart& chart);
    std::optional<CachedChart> get(const QString& filePath, DataType dt) const;
    QVector<QPair<QString, CachedChart>> getAll() const;
    QVector<QPair<QString, CachedChart>> getVisible() const;
    void remove(const QString& filePath, DataType dt);
    void removeAll(const QString& filePath);
    void removeByDataType(DataType dt);
    void clear();
    QStringList filePaths() const;

private:
    mutable std::mutex m_mutex;
    QHash<QString, QHash<DataType, CachedChart>> m_cache;
};
