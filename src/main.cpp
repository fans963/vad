#include <QApplication>
#include <QIcon>
#include "app/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("VAD"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("VAD"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("audio-x-generic")));

    MainWindow w;
    w.show();

    return app.exec();
}
