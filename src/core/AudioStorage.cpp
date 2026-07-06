#include "AudioStorage.h"

void AudioStorage::save(const QString& filePath, AudioData audio) {
    m_store[filePath] = std::move(audio);
}

std::optional<AudioData> AudioStorage::load(const QString& filePath) const {
    auto it = m_store.find(filePath);
    if (it != m_store.end()) return *it;
    return std::nullopt;
}

void AudioStorage::remove(const QString& filePath) {
    m_store.remove(filePath);
}

bool AudioStorage::contains(const QString& filePath) const {
    return m_store.contains(filePath);
}

QStringList AudioStorage::allKeys() const {
    return m_store.keys();
}
