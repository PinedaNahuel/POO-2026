#include "modobloqueado.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

ModoBloqueado::ModoBloqueado(const AppConfig &config, EventLogger &logger, QWidget *parent)
    : Pantalla(parent), m_config(config), m_logger(logger) {
    inicializarUI();
    conectarEventos();
}

void ModoBloqueado::inicializarUI() {
    setWindowTitle("Editor Multilenguaje - Bloqueado");
    setMinimumSize(520, 320);
    setFocusPolicy(Qt::StrongFocus);

    m_titulo = new QLabel("Modo bloqueado");
    m_titulo->setStyleSheet("font-size: 26px; font-weight: 700; color: #b00020;");

    m_detalle = new QLabel;
    m_detalle->setStyleSheet("font-size: 16px;");

    auto *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(m_titulo, 0, Qt::AlignCenter);
    layout->addWidget(m_detalle, 0, Qt::AlignCenter);
    layout->addStretch();
    setLayout(layout);
}

void ModoBloqueado::conectarEventos() {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ModoBloqueado::actualizarCuentaRegresiva);
}

void ModoBloqueado::cargarDatos() {
    m_detalle->setText("Intentelo nuevamente en unos segundos");
}

bool ModoBloqueado::validarEstado() {
    return m_segundosRestantes >= 0;
}

void ModoBloqueado::registrarEvento(const QString &descripcion) {
    m_logger.log("[Bloqueado] " + descripcion);
}

void ModoBloqueado::iniciarBloqueo() {
    m_segundosRestantes = m_config.tiempoBloqueoSegundos;
    m_detalle->setText("Tiempo restante: " + QString::number(m_segundosRestantes) + " segundos");
    registrarEvento("Inicio de bloqueo por " + QString::number(m_segundosRestantes) + " segundos");
    m_timer->start(1000);
}

void ModoBloqueado::keyPressEvent(QKeyEvent *event) {
    Q_UNUSED(event)
    registrarEvento("Tecla ignorada durante bloqueo");
}

void ModoBloqueado::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event)
    registrarEvento("Click ignorado durante bloqueo");
}

void ModoBloqueado::resizeEvent(QResizeEvent *event) {
    registrarEvento("Resize modo bloqueado a " + QString::number(event->size().width()) + "x" + QString::number(event->size().height()));
    Pantalla::resizeEvent(event);
}

void ModoBloqueado::closeEvent(QCloseEvent *event) {
    const auto r = QMessageBox::question(this, "Salir", "La aplicacion esta bloqueada. Desea salir?");
    if (r == QMessageBox::Yes) {
        registrarEvento("Cierre de aplicacion desde modo bloqueado");
        event->accept();
    } else {
        event->ignore();
    }
}

void ModoBloqueado::focusInEvent(QFocusEvent *event) {
    registrarEvento("Modo bloqueado recibio foco");
    Pantalla::focusInEvent(event);
}

void ModoBloqueado::focusOutEvent(QFocusEvent *event) {
    registrarEvento("Modo bloqueado perdio foco");
    Pantalla::focusOutEvent(event);
}

void ModoBloqueado::actualizarCuentaRegresiva() {
    --m_segundosRestantes;
    if (m_segundosRestantes <= 0) {
        m_timer->stop();
        m_detalle->setText("Bloqueo finalizado");
        registrarEvento("Fin de bloqueo");
        emit bloqueoFinalizado();
        return;
    }

    m_detalle->setText("Tiempo restante: " + QString::number(m_segundosRestantes) + " segundos");
}
