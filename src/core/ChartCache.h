#pragma once

#include "Types.h"
#include <QHash>
#include <QVector>
#include <optional>

// ── In-memory cached chart storage ──────────────────────────────────────────
// Each file path maps to multiple CachedChart entries (one per DataType).
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
    QStringList filePaths() const; // unique file paths

private:
    // filePath -> list of (DataType, CachedChart)
    QHash<QString, QHash<DataType, CachedChart>> m_cache;
};
