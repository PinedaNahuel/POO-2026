#include "appcontroller.h"

#include "editorprincipal.h"
#include "eventlogger.h"
#include "loginscreen.h"
#include "modobloqueado.h"
#include "pantalla.h"

AppController::AppController(const AppConfig &config, EventLogger &logger, QObject *parent)
    : QObject(parent), m_config(config), m_logger(logger) {
    m_login = new LoginScreen(m_config, m_logger);
    m_bloqueado = new ModoBloqueado(m_config, m_logger);
    m_editor = new EditorPrincipal(m_config, m_logger);

    connect(m_login, &LoginScreen::loginExitoso, this, &AppController::irAEditor);
    connect(m_login, &LoginScreen::bloqueoSolicitado, this, &AppController::irABloqueado);
    connect(m_bloqueado, &ModoBloqueado::bloqueoFinalizado, this, &AppController::finalizarBloqueo);
}

AppController::~AppController() {
    delete m_login;
    delete m_bloqueado;
    delete m_editor;
}

void AppController::iniciar() {
    cambiarPantalla(m_login, false);
}

void AppController::irAEditor() {
    cambiarPantalla(m_editor, true);
}

void AppController::irABloqueado() {
    m_bloqueado->iniciarBloqueo();
    cambiarPantalla(m_bloqueado, false);
}

void AppController::finalizarBloqueo() {
    m_login->resetIntentos();
    cambiarPantalla(m_login, false);
}

void AppController::cambiarPantalla(Pantalla *siguiente, bool fullScreen) {
    if (!siguiente) {
        return;
    }

    if (m_actual) {
        m_actual->hide();
    }

    m_actual = siguiente;
    m_actual->cargarDatos();

    if (fullScreen) {
        m_actual->showFullScreen();
    } else {
        m_actual->showNormal();
        m_actual->resize(720, 450);
        m_actual->raise();
    }

    m_actual->activateWindow();
}
