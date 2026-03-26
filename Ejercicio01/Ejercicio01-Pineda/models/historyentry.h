#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include <QString>
#include <QJsonObject>

class HistoryEntry
{
public:
    QString timestamp;
    QString action;
    QString detail;

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["timestamp"] = timestamp;
        obj["action"] = action;
        obj["detail"] = detail;
        return obj;
    }

    static HistoryEntry fromJson(const QJsonObject &obj)
    {
        HistoryEntry entry;
        entry.timestamp = obj["timestamp"].toString();
        entry.action = obj["action"].toString();
        entry.detail = obj["detail"].toString();
        return entry;
    }
};

#endif // HISTORYENTRY_H
