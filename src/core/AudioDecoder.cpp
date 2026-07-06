#include "AudioDecoder.h"

#include <sndfile.h>
#include <QTemporaryFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

AudioData AudioDecoder::decode(const QString& /*filePath*/,
                                const QByteArray& rawData,
                                const QString& format)
{
    QTemporaryFile tf(QDir::tempPath() + QStringLiteral("/vad_XXXXXX.%1").arg(format));
    tf.setAutoRemove(true);
    if (!tf.open()) { qWarning() << "[Decoder] temp file failed"; return {}; }
    tf.write(rawData);
    tf.flush();
    tf.seek(0);
    return decodeFile(tf.fileName());
}

AudioData AudioDecoder::decodeFile(const QString& filePath)
{
    AudioData result;
    qDebug() << "[Decoder] libsndfile:" << filePath;

    SF_INFO sfinfo{};
    SNDFILE* sf = sf_open(filePath.toUtf8().constData(), SFM_READ, &sfinfo);
    if (!sf) {
        qWarning() << "[Decoder] sf_open failed:" << sf_strerror(nullptr);
        return result;
    }

    qDebug() << "[Decoder] frames:" << sfinfo.frames
             << "samplerate:" << sfinfo.samplerate
             << "channels:" << sfinfo.channels
             << "format:" << sfinfo.format;

    // Read all frames as float
    int channels = sfinfo.channels;
    sf_count_t totalFrames = sfinfo.frames;
    QVector<float> interleaved(totalFrames * channels);
    sf_count_t read = sf_readf_float(sf, interleaved.data(), totalFrames);
    sf_close(sf);

    if (read <= 0) {
        qWarning() << "[Decoder] sf_readf_float read 0 frames";
        return result;
    }

    // Mix down to mono
    result.samples.resize(read);
    for (sf_count_t i = 0; i < read; ++i) {
        float sum = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            sum += interleaved[i * channels + ch];
        result.samples[i] = sum / (float)channels;
    }

    result.info.filePath = filePath;
    result.info.format = QFileInfo(filePath).suffix().toLower();
    result.info.sampleRate = sfinfo.samplerate;
    result.info.channels = channels;
    result.info.sampleCount = result.samples.size();
    if (sfinfo.samplerate > 0)
        result.info.durationSecs = (float)result.samples.size() / sfinfo.samplerate;

    qDebug() << "[Decoder] done:" << result.samples.size() << "samples,"
             << result.info.sampleRate << "Hz," << channels << "ch";
    return result;
}
