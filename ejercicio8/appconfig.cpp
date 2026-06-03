#include "appconfig.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>

AppConfig AppConfig::cargar() {
    AppConfig config;

    const QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    if (!QFile::exists(configPath)) {
        QSettings writer(configPath, QSettings::IniFormat);
        writer.setValue("auth/usuario", config.usuarioInicial);
        writer.setValue("auth/clave", config.claveInicial);
        writer.setValue("auth/tiempoBloqueoSegundos", config.tiempoBloqueoSegundos);
        writer.setValue("editor/lenguajePorDefecto", config.lenguajePorDefecto);
        writer.setValue("editor/rutaExportacion", config.rutaExportacion);
        writer.setValue("app/rutaLog", config.rutaLog);
    }

    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.mkpath("logs");
    appDir.mkpath("salidas");

    QSettings settings(configPath, QSettings::IniFormat);

    config.usuarioInicial = settings.value("auth/usuario", config.usuarioInicial).toString();
    config.claveInicial = settings.value("auth/clave", config.claveInicial).toString();
    config.tiempoBloqueoSegundos = settings.value("auth/tiempoBloqueoSegundos", config.tiempoBloqueoSegundos).toInt();
    config.lenguajePorDefecto = settings.value("editor/lenguajePorDefecto", config.lenguajePorDefecto).toString();
    config.rutaExportacion = settings.value("editor/rutaExportacion", config.rutaExportacion).toString();
    config.rutaLog = settings.value("app/rutaLog", config.rutaLog).toString();

    if (config.usuarioInicial.trimmed().isEmpty()) {
        config.usuarioInicial = "admin";
    }
    if (config.claveInicial.isEmpty()) {
        config.claveInicial = "1234";
    }
    if (config.tiempoBloqueoSegundos <= 0) {
        config.tiempoBloqueoSegundos = 15;
    }
    if (config.lenguajePorDefecto.isEmpty()) {
        config.lenguajePorDefecto = "C++";
    }
    if (config.rutaExportacion.isEmpty()) {
        config.rutaExportacion = "salidas/codigo.jpg";
    }
    if (config.rutaLog.isEmpty()) {
        config.rutaLog = "logs/eventos.log";
    }

    return config;
}
