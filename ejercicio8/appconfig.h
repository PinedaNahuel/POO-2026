#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

struct AppConfig {
    QString usuarioInicial = "admin";
    QString claveInicial = "1234";
    int tiempoBloqueoSegundos = 15;
    QString lenguajePorDefecto = "C++";
    QString rutaExportacion = "salidas/codigo.jpg";
    QString rutaLog = "logs/eventos.log";

    static AppConfig cargar();
};

#endif // APPCONFIG_H
