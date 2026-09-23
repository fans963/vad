#pragma once

#include "Types.h"
#include <QString>
#include <QByteArray>

// ── Multi-format audio decoder using miniaudio ──────────────────────────────
// Supports WAV, FLAC, OGG, MP3, etc.
class AudioDecoder {
public:
    AudioData decode(const QString& filePath, const QByteArray& rawData,
                     const QString& format);
    AudioData decodeFile(const QString& filePath);
};
