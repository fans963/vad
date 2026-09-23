#include "AudioStorage.h"

void AudioStorage::save(const QString& filePath, AudioData audio) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_store[filePath] = std::move(audio);
}

std::optional<AudioData> AudioStorage::load(const QString& filePath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_store.find(filePath);
    if (it != m_store.end()) return *it;
    return std::nullopt;
}

void AudioStorage::remove(const QString& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_store.remove(filePath);
}

bool AudioStorage::contains(const QString& filePath) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_store.contains(filePath);
}

QStringList AudioStorage::allKeys() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_store.keys();
}
