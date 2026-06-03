#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    static void init(const QString &baseDir);
    static void log(const QString &descripcion);

private:
    static QString s_logPath;
};

#endif // LOGGER_H
