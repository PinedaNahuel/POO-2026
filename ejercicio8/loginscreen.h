#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

#include "appconfig.h"
#include "eventlogger.h"
#include "pantalla.h"

class QLineEdit;
class QLabel;
class QPushButton;

class LoginScreen : public Pantalla {
    Q_OBJECT

public:
    explicit LoginScreen(const AppConfig &config, EventLogger &logger, QWidget *parent = nullptr);

    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() override;
    void registrarEvento(const QString &descripcion) override;

    void resetIntentos();

signals:
    void loginExitoso();
    void bloqueoSolicitado();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void intentarLogin();

private:
    const AppConfig &m_config;
    EventLogger &m_logger;

    QLineEdit *m_usuario = nullptr;
    QLineEdit *m_clave = nullptr;
    QLabel *m_estado = nullptr;
    QPushButton *m_btnEntrar = nullptr;

    int m_intentosFallidos = 0;
};

#endif // LOGINSCREEN_H
