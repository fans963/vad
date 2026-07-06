#pragma once

#include <QString>
#include <QDebug>

#define _T(x) x
#define AfxMessageBox(x) qDebug() << x
#define LPCTSTR const char*

class CFileFind {
public:
    CFileFind() {}
    bool FindFile(const QString& path) { return false; }
    bool FindNextFileW() { return false; }
    QString GetFileTitle() { return ""; }
};
