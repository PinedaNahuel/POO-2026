#include "jsonstorage.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

JsonStorage::JsonStorage(const QString &basePath)
    : m_basePath(basePath)
{
}

QByteArray JsonStorage::readFile(const QString &filePath) const
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return QByteArray();

    return file.readAll();
}

bool JsonStorage::writeFile(const QString &filePath, const QByteArray &data) const
{
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    file.write(data);
    file.close();
    return true;
}

QList<User> JsonStorage::loadUsers() const
{
    QList<User> users;
    QByteArray data = readFile(m_basePath + "users.json");

    if (data.isEmpty())
        return users;

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray())
        return users;

    QJsonArray array = doc.array();

    for (const QJsonValue &value : array)
    {
        if (value.isObject())
            users.append(User::fromJson(value.toObject()));
    }

    return users;
}

bool JsonStorage::saveUsers(const QList<User> &users) const
{
    QJsonArray array;

    for (const User &user : users)
    {
        array.append(user.toJson());
    }

    QJsonDocument doc(array);
    return writeFile(m_basePath + "users.json", doc.toJson(QJsonDocument::Indented));
}

QList<Task> JsonStorage::loadTasks() const
{
    QList<Task> tasks;
    QByteArray data = readFile(m_basePath + "tasks.json");

    if (data.isEmpty())
        return tasks;

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray())
        return tasks;

    QJsonArray array = doc.array();

    for (const QJsonValue &value : array)
    {
        if (value.isObject())
            tasks.append(Task::fromJson(value.toObject()));
    }

    return tasks;
}

bool JsonStorage::saveTasks(const QList<Task> &tasks) const
{
    QJsonArray array;

    for (const Task &task : tasks)
    {
        array.append(task.toJson());
    }

    QJsonDocument doc(array);
    return writeFile(m_basePath + "tasks.json", doc.toJson(QJsonDocument::Indented));
}

QList<HistoryEntry> JsonStorage::loadHistory() const
{
    QList<HistoryEntry> history;
    QByteArray data = readFile(m_basePath + "history.json");

    if (data.isEmpty())
        return history;

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray())
        return history;

    QJsonArray array = doc.array();

    for (const QJsonValue &value : array)
    {
        if (value.isObject())
            history.append(HistoryEntry::fromJson(value.toObject()));
    }

    return history;
}

bool JsonStorage::saveHistory(const QList<HistoryEntry> &history) const
{
    QJsonArray array;

    for (const HistoryEntry &entry : history)
    {
        array.append(entry.toJson());
    }

    QJsonDocument doc(array);
    return writeFile(m_basePath + "history.json", doc.toJson(QJsonDocument::Indented));
}
