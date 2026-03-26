#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include <QString>
#include <QList>

#include "../models/user.h"
#include "../models/task.h"
#include "../models/historyentry.h"

class JsonStorage
{
public:
    explicit JsonStorage(const QString &basePath = "data/");

    QList<User> loadUsers() const;
    bool saveUsers(const QList<User> &users) const;

    QList<Task> loadTasks() const;
    bool saveTasks(const QList<Task> &tasks) const;

    QList<HistoryEntry> loadHistory() const;
    bool saveHistory(const QList<HistoryEntry> &history) const;

private:
    QString m_basePath;

    QByteArray readFile(const QString &filePath) const;
    bool writeFile(const QString &filePath, const QByteArray &data) const;
};

#endif // JSONSTORAGE_H
