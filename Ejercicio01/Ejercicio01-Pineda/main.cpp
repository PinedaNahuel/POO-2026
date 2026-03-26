#include "login.h"
#include "services/jsonstorage.h"

#include <QApplication>
#include <QDebug>

#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    JsonStorage storage( "D:/Usuario Lab/Documentos/Ejercicio01-Pineda/data/" );
    JsonStorage storage( "../data/" );

    qDebug() << QDir::currentPath();

    QList<User> users = storage.loadUsers();

    qDebug() << "Cantidad de usuarios:" << users.size();

    for (const User &user : users)
    {
        qDebug() << "Usuario:" << user.username << "-" << user.displayName;
    }

    Login w;
    w.show();

    return a.exec();
}
