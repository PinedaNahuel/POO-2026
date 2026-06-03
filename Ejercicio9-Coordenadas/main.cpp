#include <QApplication>
#include <QDir>
#include <QDebug>
#include "admindb.h"
#include "login.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // La base de datos se ubica junto al ejecutable
    QString dbPath = QCoreApplication::applicationDirPath() + "/dibujo.db";
    qDebug() << "Base de datos en:" << dbPath;

    AdminDB *adminDB = new AdminDB();

    if (!adminDB->conectar(dbPath)) {
        qDebug() << "No se pudo abrir la base de datos.";
        return 1;
    }

    if (!adminDB->inicializarTablas()) {
        qDebug() << "Error al inicializar las tablas.";
        return 1;
    }

    // Mostrar login; si el usuario cancela, cerramos la aplicación
    Login login(adminDB);
    if (login.exec() != QDialog::Accepted) {
        delete adminDB;
        return 0;
    }

    MainWindow w(adminDB);
    w.show();

    int resultado = a.exec();

    delete adminDB;
    return resultado;
}
