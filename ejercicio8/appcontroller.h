#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "appconfig.h"

#include <QObject>

class EventLogger;
class Pantalla;
class LoginScreen;
class ModoBloqueado;
class EditorPrincipal;

class AppController : public QObject {
    Q_OBJECT

public:
    explicit AppController(const AppConfig &config, EventLogger &logger, QObject *parent = nullptr);
    ~AppController() override;

    void iniciar();

private slots:
    void irAEditor();
    void irABloqueado();
    void finalizarBloqueo();

private:
    void cambiarPantalla(Pantalla *siguiente, bool fullScreen);

    const AppConfig &m_config;
    EventLogger &m_logger;

    Pantalla *m_actual = nullptr;
    LoginScreen *m_login = nullptr;
    ModoBloqueado *m_bloqueado = nullptr;
    EditorPrincipal *m_editor = nullptr;
};

#endif // APPCONTROLLER_H
