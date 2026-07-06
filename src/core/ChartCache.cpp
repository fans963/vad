#include "ChartCache.h"

void ChartCache::add(const QString& filePath, const CachedChart& chart) {
    m_cache[filePath][chart.dataType] = chart;
}

std::optional<CachedChart> ChartCache::get(const QString& filePath, DataType dt) const {
    auto it1 = m_cache.find(filePath);
    if (it1 == m_cache.end()) return std::nullopt;
    auto it2 = it1->find(dt);
    if (it2 == it1->end()) return std::nullopt;
    return *it2;
}

QVector<QPair<QString, CachedChart>> ChartCache::getAll() const {
    QVector<QPair<QString, CachedChart>> result;
    for (auto it1 = m_cache.begin(); it1 != m_cache.end(); ++it1)
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
            result.append({it1.key(), *it2});
    return result;
}

QVector<QPair<QString, CachedChart>> ChartCache::getVisible() const {
    QVector<QPair<QString, CachedChart>> result;
    for (auto it1 = m_cache.begin(); it1 != m_cache.end(); ++it1)
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
            if (it2->visible)
                result.append({it1.key(), *it2});
    return result;
}

void ChartCache::remove(const QString& filePath, DataType dt) {
    auto it = m_cache.find(filePath);
    if (it != m_cache.end()) {
        it->remove(dt);
        if (it->isEmpty()) m_cache.erase(it);
    }
}

void ChartCache::removeAll(const QString& filePath) {
    m_cache.remove(filePath);
}

void ChartCache::removeByDataType(DataType dt) {
    QList<QString> toClear;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        it->remove(dt);
        if (it->isEmpty()) toClear.append(it.key());
    }
    for (const auto& k : toClear) m_cache.remove(k);
}

void ChartCache::clear() { m_cache.clear(); }

QStringList ChartCache::filePaths() const {
    QStringList keys = m_cache.keys();
    keys.sort();
    keys.removeDuplicates();
    return keys;
}
