#ifndef ADMINDB_H
#define ADMINDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class AdminDB : public QObject
{
    Q_OBJECT

public:
    explicit AdminDB(QObject *parent = nullptr);

    bool conectar(const QString &archivoSqlite);
    QSqlDatabase getDB();
    bool inicializarTablas();
    bool validarUsuario(const QString &usuario, const QString &clave,
                        QString &nombre, QString &apellido);
    bool crearUsuario(const QString &usuario, const QString &clave,
                      const QString &nombre, const QString &apellido,
                      const QString &mail, QString &error);
    bool registrarLog(const QString &evento);

private:
    QSqlDatabase db;
};

#endif // ADMINDB_H
