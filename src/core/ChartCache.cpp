#include "ChartCache.h"

void ChartCache::add(const QString& filePath, const CachedChart& chart) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache[filePath][chart.dataType] = chart;
}

std::optional<CachedChart> ChartCache::get(const QString& filePath, DataType dt) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it1 = m_cache.find(filePath);
    if (it1 == m_cache.end()) return std::nullopt;
    auto it2 = it1->find(dt);
    if (it2 == it1->end()) return std::nullopt;
    return *it2;
}

QVector<QPair<QString, CachedChart>> ChartCache::getAll() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QVector<QPair<QString, CachedChart>> result;
    for (auto it1 = m_cache.begin(); it1 != m_cache.end(); ++it1)
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
            result.append({it1.key(), *it2});
    return result;
}

QVector<QPair<QString, CachedChart>> ChartCache::getVisible() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QVector<QPair<QString, CachedChart>> result;
    for (auto it1 = m_cache.begin(); it1 != m_cache.end(); ++it1)
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
            if (it2->visible)
                result.append({it1.key(), *it2});
    return result;
}

void ChartCache::remove(const QString& filePath, DataType dt) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_cache.find(filePath);
    if (it != m_cache.end()) {
        it->remove(dt);
        if (it->isEmpty()) m_cache.erase(it);
    }
}

void ChartCache::removeAll(const QString& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.remove(filePath);
}

void ChartCache::removeByDataType(DataType dt) {
    std::lock_guard<std::mutex> lock(m_mutex);
    QList<QString> toClear;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        it->remove(dt);
        if (it->isEmpty()) toClear.append(it.key());
    }
    for (const auto& k : toClear) m_cache.remove(k);
}

void ChartCache::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cache.clear();
}

QStringList ChartCache::filePaths() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    QStringList keys = m_cache.keys();
    keys.sort();
    keys.removeDuplicates();
    return keys;
}
