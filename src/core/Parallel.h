#pragma once

#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

// ── Global thread pool ──────────────────────────────────────────────────────
inline QThreadPool* globalThreadPool() {
    return QThreadPool::globalInstance();
}

// ── parallelFor: Qt thread pool based parallel for-loop ─────────────────────
template <typename F>
void parallelFor(int count, F&& callable) {
    if (count <= 0) return;

    // Build index vector
    QVector<int> indices(count);
    for (int i = 0; i < count; ++i)
        indices[i] = i;

    QtConcurrent::blockingMap(indices, std::forward<F>(callable));
}

// ── parallelMap: blocking mapped (like rayon par_iter().map()) ──────────────
template <typename T, typename F>
QVector<decltype(std::declval<F>()(std::declval<T>()))>
parallelMap(const QVector<T>& input, F&& callable) {
    return QtConcurrent::blockingMapped(input, std::forward<F>(callable));
}
