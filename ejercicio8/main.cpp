#include <QApplication>

#include "appconfig.h"
#include "appcontroller.h"
#include "eventlogger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // Estilo global para mejorar visibilidad: fondo claro y texto oscuro
    app.setStyleSheet(R"(
        QWidget { background-color: #f5f7fa; color: #222; }
        QPlainTextEdit { background: #ffffff; color: #111; }
        QLabel { color: #111; }
        QPushButton { background-color: #1f6feb; color: #fff; border-radius: 4px; padding: 4px 8px; }
    )");

    const AppConfig config = AppConfig::cargar();
    EventLogger logger(config.rutaLog);
    logger.log("Aplicacion iniciada en modo offline");

    AppController controlador(config, logger);
    controlador.iniciar();

    return app.exec();
}
