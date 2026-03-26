#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class User
{
public:
    QString username;
    QString password;
    QString displayName;

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["username"] = username;
        obj["password"] = password;
        obj["displayName"] = displayName;
        return obj;
    }

    static User fromJson(const QJsonObject &obj)
    {
        User user;
        user.username = obj["username"].toString();
        user.password = obj["password"].toString();
        user.displayName = obj["displayName"].toString();
        return user;
    }
};

#endif // USER_H
