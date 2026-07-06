#include "HomePanel.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QUrl>
#include <QDesktopServices>

HomePanel::HomePanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* title = new QLabel(QStringLiteral("VAD - Voice Activity Detection"));
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    layout->addSpacing(10);

    auto* subtitle = new QLabel(
        QStringLiteral("Real-time interactive audio analysis tool\n"
                       "Drag & drop or click + to load audio files"));
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-size: 14px; color: #666;");
    layout->addWidget(subtitle);

    layout->addSpacing(20);

    auto* devLabel = new QLabel(QStringLiteral("Developers: Fans963 & 津哥"));
    devLabel->setAlignment(Qt::AlignCenter);
    devLabel->setStyleSheet("font-size: 14px;");
    layout->addWidget(devLabel);

    auto* linkLabel = new QLabel(
        QStringLiteral("<a href='https://github.com/fans963/vad_flutter_and_rust'>"
                       "github.com/fans963/vad_flutter_and_rust</a>"));
    linkLabel->setAlignment(Qt::AlignCenter);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setStyleSheet("font-size: 13px;");
    layout->addWidget(linkLabel);

    layout->addSpacing(20);

    auto* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    layout->addWidget(line);

    layout->addSpacing(10);

    auto* infoLabel = new QLabel(
        QStringLiteral("Features:\n"
                       "• Multi-format audio support (WAV, MP3, FLAC, etc.)\n"
                       "• Audio waveform & spectrum visualization\n"
                       "• Energy & Zero-Crossing Rate analysis\n"
                       "• Multi-algorithm VAD (Energy-based, ZCR-based)\n"
                       "• Audio playback with speed control\n"
                       "• Drag & drop file loading"));
    infoLabel->setStyleSheet("font-size: 12px; color: #555;");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);
}
