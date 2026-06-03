#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

#include <QString>

class EventLogger {
public:
    explicit EventLogger(const QString &rutaRelativa);
    void log(const QString &mensaje) const;

private:
    QString m_rutaAbsoluta;
};

#endif // EVENTLOGGER_H
