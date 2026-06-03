#include "eventlogger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

EventLogger::EventLogger(const QString &rutaRelativa) {
    m_rutaAbsoluta = QCoreApplication::applicationDirPath() + "/" + rutaRelativa;

    QFileInfo info(m_rutaAbsoluta);
    QDir().mkpath(info.path());
}

void EventLogger::log(const QString &mensaje) const {
    QFile file(m_rutaAbsoluta);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
        << " | " << mensaje << "\n";
}
