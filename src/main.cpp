#include <QApplication>
#include <QIcon>
#include "app/MainWindow.h"

#if defined(_WIN32)
#include <mimalloc.h>
#endif

int main(int argc, char* argv[]) {
#if defined(_WIN32)
    mi_version();
#endif
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MyWave"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("MyWave"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("audio-x-generic")));

    MainWindow w;
    w.show();

    return app.exec();
}
