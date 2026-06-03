#include "loginscreen.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QResizeEvent>

LoginScreen::LoginScreen(const AppConfig &config, EventLogger &logger, QWidget *parent)
    : Pantalla(parent), m_config(config), m_logger(logger) {
    inicializarUI();
    conectarEventos();
}

void LoginScreen::inicializarUI() {
    setWindowTitle("Editor Multilenguaje - Login");
    setMinimumSize(520, 320);
    setFocusPolicy(Qt::StrongFocus);

    auto *titulo = new QLabel("Login inicial");
    titulo->setStyleSheet("font-size: 24px; font-weight: 700;");

    auto *usuarioLabel = new QLabel("Usuario:");
    auto *claveLabel = new QLabel("Clave:");

    m_usuario = new QLineEdit;
    m_usuario->setPlaceholderText("admin");

    m_clave = new QLineEdit;
    m_clave->setEchoMode(QLineEdit::Password);
    m_clave->setPlaceholderText("1234");

    m_btnEntrar = new QPushButton("Ingresar");

    m_estado = new QLabel("Ingrese sus credenciales");
    m_estado->setStyleSheet("color: #234; font-size: 12px;");

    auto *layout = new QGridLayout;
    layout->addWidget(titulo, 0, 0, 1, 2);
    layout->addWidget(usuarioLabel, 1, 0);
    layout->addWidget(m_usuario, 1, 1);
    layout->addWidget(claveLabel, 2, 0);
    layout->addWidget(m_clave, 2, 1);
    layout->addWidget(m_btnEntrar, 3, 0, 1, 2);
    layout->addWidget(m_estado, 4, 0, 1, 2);
    setLayout(layout);
}

void LoginScreen::conectarEventos() {
    connect(m_btnEntrar, &QPushButton::clicked, this, &LoginScreen::intentarLogin);
    connect(m_clave, &QLineEdit::returnPressed, this, &LoginScreen::intentarLogin);
}

void LoginScreen::cargarDatos() {
    m_usuario->setText(m_config.usuarioInicial);
    m_clave->clear();
    m_estado->setText("Ingrese sus credenciales");
}

bool LoginScreen::validarEstado() {
    return !m_usuario->text().trimmed().isEmpty() && !m_clave->text().isEmpty();
}

void LoginScreen::registrarEvento(const QString &descripcion) {
    m_logger.log("[Login] " + descripcion);
}

void LoginScreen::resetIntentos() {
    m_intentosFallidos = 0;
}

void LoginScreen::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        registrarEvento("Atajo Enter en pantalla de login");
        intentarLogin();
        return;
    }
    Pantalla::keyPressEvent(event);
}

void LoginScreen::mousePressEvent(QMouseEvent *event) {
    registrarEvento("Click en login en posicion " + QString::number(event->position().x()) + "," + QString::number(event->position().y()));
    Pantalla::mousePressEvent(event);
}

void LoginScreen::resizeEvent(QResizeEvent *event) {
    registrarEvento("Resize login a " + QString::number(event->size().width()) + "x" + QString::number(event->size().height()));
    Pantalla::resizeEvent(event);
}

void LoginScreen::closeEvent(QCloseEvent *event) {
    const auto r = QMessageBox::question(this, "Salir", "Desea salir de la aplicacion?");
    if (r == QMessageBox::Yes) {
        registrarEvento("Cierre de aplicacion desde login");
        event->accept();
    } else {
        event->ignore();
    }
}

void LoginScreen::focusInEvent(QFocusEvent *event) {
    registrarEvento("Login recibio foco");
    Pantalla::focusInEvent(event);
}

void LoginScreen::focusOutEvent(QFocusEvent *event) {
    registrarEvento("Login perdio foco");
    Pantalla::focusOutEvent(event);
}

void LoginScreen::intentarLogin() {
    if (!validarEstado()) {
        m_estado->setText("Usuario y clave no pueden estar vacios");
        m_estado->setStyleSheet("color: #b00020;");
        registrarEvento("Intento invalido por campos vacios");
        return;
    }

    const bool ok = m_usuario->text().trimmed() == m_config.usuarioInicial
        && m_clave->text() == m_config.claveInicial;

    if (ok) {
        m_estado->setText("Login correcto");
        m_estado->setStyleSheet("color: #0b7a2b;");
        registrarEvento("Login exitoso");
        resetIntentos();
        emit loginExitoso();
        return;
    }

    ++m_intentosFallidos;
    registrarEvento("Login fallido. Intento " + QString::number(m_intentosFallidos));
    m_estado->setText("Credenciales invalidas (" + QString::number(m_intentosFallidos) + "/3)");
    m_estado->setStyleSheet("color: #b00020;");

    if (m_intentosFallidos >= 3) {
        registrarEvento("Se activa modo bloqueado por 3 intentos fallidos");
        emit bloqueoSolicitado();
    }
}
