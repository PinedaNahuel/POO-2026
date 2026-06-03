#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

QString Logger::s_logPath;

void Logger::init(const QString &baseDir)
{
    QDir dir(baseDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    s_logPath = dir.filePath("eventos.log");
}

void Logger::log(const QString &descripcion)
{
    if (s_logPath.isEmpty()) {
        s_logPath = QDir::current().filePath("eventos.log");
    }

    QFile file(s_logPath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
           << " | " << descripcion << "\n";
}
