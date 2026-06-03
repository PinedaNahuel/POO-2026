#ifndef LOGIN_H
#define LOGIN_H

#include "appconfig.h"
#include "climaservicio.h"
#include "pantalla.h"

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Ventana;

class Login : public Pantalla
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

    void configurarPantalla() override;

private slots:
    void on_btnIngresar_clicked();
    void onClimaActualizado(const ClimaInfo &info);
    void onErrorAmigable(const QString &mensaje);
    void onBackgroundDownloaded();
    void onLockTimerTick();
    void onClockTick();

private:
    Ui::Login *ui;

    AppConfig m_config;
    ClimaServicio *m_climaServicio = nullptr;
    QNetworkAccessManager m_backgroundManager;

    QTimer m_lockTimer;
    QTimer m_clockTimer;

    int m_intentosFallidos = 0;
    QDateTime m_bloqueoHasta;

    bool m_backgroundReady = false;
    bool m_loginAprobadoPendiente = false;
    QString m_backgroundCachePath;
    ClimaInfo m_climaActual;

    Ventana *m_ventana = nullptr;

    void iniciarDescargaFondo();
    void abrirVentanaSiLista();
    void setLoginEnabled(bool enabled);
};

#endif // LOGIN_H
