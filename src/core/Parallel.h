#pragma once

#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QVector>
#include <QPair>
#include <algorithm>

// ── Global thread pool ──────────────────────────────────────────────────────
inline QThreadPool* globalThreadPool() {
    return QThreadPool::globalInstance();
}

// ── parallelFor: Chunked Qt thread pool based parallel for-loop ────────────
// Eliminates the massive overhead of allocating per-index tasks by grouping
// iterations into coarse-grained chunks (e.g. 1-4 chunks per thread).
template <typename F>
void parallelFor(int count, F&& callable, int minChunkSize = 256) {
    if (count <= 0) return;

    const int maxThreads = QThreadPool::globalInstance()->maxThreadCount();
    if (maxThreads <= 1 || count < minChunkSize * 2) {
        for (int i = 0; i < count; ++i) {
            callable(i);
        }
        return;
    }

    const int targetChunks = std::min(maxThreads * 2, std::max(2, (count + minChunkSize - 1) / minChunkSize));
    const int chunkSize = (count + targetChunks - 1) / targetChunks;

    QVector<QPair<int, int>> chunks;
    chunks.reserve(targetChunks);
    for (int start = 0; start < count; start += chunkSize) {
        chunks.append({start, std::min(start + chunkSize, count)});
    }

    QtConcurrent::blockingMap(chunks, [&callable](const QPair<int, int>& chunk) {
        for (int i = chunk.first; i < chunk.second; ++i) {
            callable(i);
        }
    });
}

// ── parallelMap: blocking mapped ────────────────────────────────────────────
template <typename T, typename F>
QVector<decltype(std::declval<F>()(std::declval<T>()))>
parallelMap(const QVector<T>& input, F&& callable) {
    return QtConcurrent::blockingMapped(input, std::forward<F>(callable));
}
