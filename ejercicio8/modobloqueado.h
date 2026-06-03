#ifndef MODOBLOQUEADO_H
#define MODOBLOQUEADO_H

#include "appconfig.h"
#include "eventlogger.h"
#include "pantalla.h"

class QLabel;
class QTimer;

class ModoBloqueado : public Pantalla {
    Q_OBJECT

public:
    explicit ModoBloqueado(const AppConfig &config, EventLogger &logger, QWidget *parent = nullptr);

    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() override;
    void registrarEvento(const QString &descripcion) override;

    void iniciarBloqueo();

signals:
    void bloqueoFinalizado();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void actualizarCuentaRegresiva();

private:
    const AppConfig &m_config;
    EventLogger &m_logger;

    QLabel *m_titulo = nullptr;
    QLabel *m_detalle = nullptr;
    QTimer *m_timer = nullptr;
    int m_segundosRestantes = 0;
};

#endif // MODOBLOQUEADO_H
