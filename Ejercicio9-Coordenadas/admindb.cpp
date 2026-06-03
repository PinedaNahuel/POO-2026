#include "admindb.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QCryptographicHash>

namespace {

QString hashClave(const QString &clave)
{
    return QString(
        QCryptographicHash::hash(clave.toUtf8(), QCryptographicHash::Sha256).toHex()
    );
}

bool pareceHashSha256(const QString &valor)
{
    if (valor.size() != 64)
        return false;

    for (const QChar &c : valor) {
        if (!((c >= '0' && c <= '9') ||
              (c >= 'a' && c <= 'f') ||
              (c >= 'A' && c <= 'F'))) {
            return false;
        }
    }
    return true;
}

}

AdminDB::AdminDB(QObject *parent)
    : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
}

bool AdminDB::conectar(const QString &archivoSqlite)
{
    db.setDatabaseName(archivoSqlite);
    if (db.open()) {
        qDebug() << "Conexión exitosa a:" << archivoSqlite;
        return true;
    }
    qDebug() << "Error al conectar:" << db.lastError().text();
    return false;
}

QSqlDatabase AdminDB::getDB()
{
    return db;
}

bool AdminDB::inicializarTablas()
{
    QSqlQuery query(db);

    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "usuario VARCHAR(30) UNIQUE,"
        "clave VARCHAR(30),"
        "nombre VARCHAR(50),"
        "apellido VARCHAR(50),"
        "mail VARCHAR(50)"
        ")"
    );
    if (!ok) {
        qDebug() << "Error creando tabla usuarios:" << query.lastError().text();
        return false;
    }

    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "evento TEXT,"
        "fecha TEXT"
        ")"
    );
    if (!ok) {
        qDebug() << "Error creando tabla logs:" << query.lastError().text();
        return false;
    }

    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS trazos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT"
        ")"
    );
    if (!ok) {
        qDebug() << "Error creando tabla trazos:" << query.lastError().text();
        return false;
    }

    ok = query.exec(
        "CREATE TABLE IF NOT EXISTS puntos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "trazo_id INTEGER,"
        "x INTEGER,"
        "y INTEGER,"
        "r INTEGER,"
        "g INTEGER,"
        "b INTEGER,"
        "grosor INTEGER"
        ")"
    );
    if (!ok) {
        qDebug() << "Error creando tabla puntos:" << query.lastError().text();
        return false;
    }

    // Insertar usuario de prueba si no existe
    query.prepare(
        "INSERT OR IGNORE INTO usuarios (usuario, clave, nombre, apellido, mail) "
        "VALUES (:usr, :clave, :nom, :ape, :mail)"
    );
    query.bindValue(":usr",   "admin");
    query.bindValue(":clave", hashClave("1234"));
    query.bindValue(":nom",   "Administrador");
    query.bindValue(":ape",   "Sistema");
    query.bindValue(":mail",  "admin@sistema.com");
    query.exec();

    // Migrar contraseñas antiguas en texto plano a SHA-256
    QSqlQuery qSel(db);
    if (qSel.exec("SELECT id, clave FROM usuarios")) {
        QSqlQuery qUpd(db);
        while (qSel.next()) {
            const int id = qSel.value(0).toInt();
            const QString claveGuardada = qSel.value(1).toString();

            if (!pareceHashSha256(claveGuardada)) {
                qUpd.prepare("UPDATE usuarios SET clave = :clave WHERE id = :id");
                qUpd.bindValue(":clave", hashClave(claveGuardada));
                qUpd.bindValue(":id", id);
                if (!qUpd.exec()) {
                    qDebug() << "Error migrando clave usuario id" << id << ":"
                             << qUpd.lastError().text();
                }
            }
        }
    }

    return true;
}

bool AdminDB::validarUsuario(const QString &usuario, const QString &clave,
                              QString &nombre, QString &apellido)
{
    QSqlQuery query(db);
    const QString claveHash = hashClave(clave);

    query.prepare(
        "SELECT nombre, apellido FROM usuarios "
        "WHERE usuario = :usr AND (clave = :claveHash OR clave = :clavePlano)"
    );
    query.bindValue(":usr", usuario);
    query.bindValue(":claveHash", claveHash);
    query.bindValue(":clavePlano", clave);

    if (!query.exec()) {
        qDebug() << "Error validarUsuario:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        nombre   = query.value(0).toString();
        apellido = query.value(1).toString();
        return true;
    }
    return false;
}

bool AdminDB::crearUsuario(const QString &usuario, const QString &clave,
                           const QString &nombre, const QString &apellido,
                           const QString &mail, QString &error)
{
    if (usuario.trimmed().isEmpty() || clave.isEmpty() ||
        nombre.trimmed().isEmpty() || apellido.trimmed().isEmpty()) {
        error = "Complete usuario, clave, nombre y apellido.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO usuarios (usuario, clave, nombre, apellido, mail) "
        "VALUES (:usr, :clave, :nom, :ape, :mail)"
    );
    query.bindValue(":usr", usuario.trimmed());
    query.bindValue(":clave", hashClave(clave));
    query.bindValue(":nom", nombre.trimmed());
    query.bindValue(":ape", apellido.trimmed());
    query.bindValue(":mail", mail.trimmed());

    if (query.exec()) {
        error.clear();
        return true;
    }

    const QString dbError = query.lastError().text();
    if (dbError.contains("UNIQUE", Qt::CaseInsensitive) ||
        dbError.contains("unique", Qt::CaseInsensitive)) {
        error = "El usuario ya existe.";
    } else {
        error = "No se pudo crear el usuario.";
    }

    qDebug() << "Error crearUsuario:" << dbError;
    return false;
}

bool AdminDB::registrarLog(const QString &evento)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO logs (evento, fecha) VALUES (:evento, :fecha)");
    query.bindValue(":evento", evento);
    query.bindValue(":fecha",  QDateTime::currentDateTime().toString(Qt::ISODate));

    bool ok = query.exec();
    if (!ok)
        qDebug() << "Error registrarLog:" << query.lastError().text();
    return ok;
}
