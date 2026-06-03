#include "login.h"
#include "ui_login.h"

#include "logger.h"
#include "ventana.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLinearGradient>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QPainter>

Login::Login(QWidget *parent)
    : Pantalla(parent)
    , ui(new Ui::Login)
    , m_config(QCoreApplication::applicationDirPath())
{
    ui->setupUi(this);
    configurarPantalla();
}

Login::~Login()
{
    delete ui;
}

void Login::configurarPantalla()
{
    setWindowTitle("Login - Ejercicio 07");
    setMinimumSize(900, 620);

    Logger::init(QDir(QCoreApplication::applicationDirPath()).filePath("logs"));
    Logger::log("Aplicacion iniciada.");

    m_config.load();
    Logger::log("Configuracion cargada desde: " + m_config.configPath);

    if (m_config.proxy.enabled && !m_config.proxy.host.isEmpty()) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_config.proxy.host, m_config.proxy.port,
                            m_config.proxy.user, m_config.proxy.password);
        m_backgroundManager.setProxy(proxy);
        Logger::log("Proxy habilitado para fondo: " + m_config.proxy.host + ":" + QString::number(m_config.proxy.port));
    }

    connect(ui->btnIngresar, &QPushButton::clicked, this, &Login::on_btnIngresar_clicked);
    connect(ui->editUsuario, &QLineEdit::returnPressed, this, &Login::on_btnIngresar_clicked);
    connect(ui->editPassword, &QLineEdit::returnPressed, this, &Login::on_btnIngresar_clicked);
    ui->btnIngresar->setDefault(true);
    ui->btnIngresar->setAutoDefault(true);

    connect(&m_lockTimer, &QTimer::timeout, this, &Login::onLockTimerTick);
    m_lockTimer.setInterval(1000);

    connect(&m_clockTimer, &QTimer::timeout, this, &Login::onClockTick);
    m_clockTimer.setInterval(1000);

    m_climaServicio = new ClimaServicio(m_config, this);
    connect(m_climaServicio, &ClimaServicio::climaActualizado, this, &Login::onClimaActualizado);
    connect(m_climaServicio, &ClimaServicio::errorAmigable, this, &Login::onErrorAmigable);
    connect(m_climaServicio, &ClimaServicio::estadoConectividad, this, [this](bool online, const QString &detalle) {
        ui->lblEstadoRed->setText(online ? "Online" : "Offline detectado");
        ui->lblEstadoRed->setStyleSheet(online ? "color: #1D6A2B;" : "color: #A35A00;");
        if (!online && !detalle.isEmpty()) {
            ui->lblMensaje->setText(detalle);
        }
    });

    m_climaServicio->actualizar();

    ui->lblMensaje->setText("Ingresa con admin / 1234");
    iniciarDescargaFondo();
}

void Login::iniciarDescargaFondo()
{
    const QDir appDir(QCoreApplication::applicationDirPath());
    const QString cacheDir = appDir.filePath("cache");
    QDir().mkpath(cacheDir);
    m_backgroundCachePath = QDir(cacheDir).filePath("fondo_principal.jpg");

    if (QFile::exists(m_backgroundCachePath)) {
        m_backgroundReady = true;
        ui->lblEstadoFondo->setText("Fondo listo (cache local)");
        Logger::log("Imagen de fondo encontrada en cache.");
        return;
    }

    ui->lblEstadoFondo->setText("Descargando fondo...");

    QNetworkRequest request(QUrl(m_config.backgroundUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtLoginClima/1.0");

    QNetworkReply *reply = m_backgroundManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &Login::onBackgroundDownloaded);
    Logger::log("Descarga de fondo iniciada: " + m_config.backgroundUrl);
}

void Login::onBackgroundDownloaded()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        return;
    }

    QByteArray bytes = reply->readAll();
    const bool error = reply->error() != QNetworkReply::NoError;
    const QString errorText = reply->errorString();
    reply->deleteLater();

    bool ok = false;
    if (!error && !bytes.isEmpty()) {
        QFile out(m_backgroundCachePath);
        if (out.open(QIODevice::WriteOnly)) {
            out.write(bytes);
            out.close();

            QPixmap probe;
            ok = probe.load(m_backgroundCachePath);
        }
    }

    if (!ok) {
        QPixmap fallback(1920, 1080);
        QPainter painter(&fallback);
        QLinearGradient grad(0, 0, fallback.width(), fallback.height());
        grad.setColorAt(0.0, QColor("#0F3A53"));
        grad.setColorAt(1.0, QColor("#1B7A8D"));
        painter.fillRect(fallback.rect(), grad);

        ok = fallback.save(m_backgroundCachePath, "JPG");

        if (error) {
            Logger::log("Error al descargar fondo: " + errorText + ". Se genero fondo local.");
        } else {
            Logger::log("Fondo descargado invalido. Se genero fondo local.");
        }
    } else {
        Logger::log("Fondo descargado y guardado en cache.");
    }

    m_backgroundReady = ok;
    ui->lblEstadoFondo->setText(ok ? "Fondo listo" : "No se pudo preparar el fondo");

    if (m_loginAprobadoPendiente && m_backgroundReady) {
        abrirVentanaSiLista();
    }
}

void Login::on_btnIngresar_clicked()
{
    if (QDateTime::currentDateTime() < m_bloqueoHasta) {
        const int secs = QDateTime::currentDateTime().secsTo(m_bloqueoHasta);
        ui->lblMensaje->setText("Login bloqueado. Reintenta en " + QString::number(secs) + " segundos.");
        return;
    }

    const QString user = ui->editUsuario->text().trimmed();
    const QString pass = ui->editPassword->text();

    if (user == "admin" && pass == "1234") {
        Logger::log("Usuario valido autenticado: " + user);
        ui->lblMensaje->setText("Usuario valido. Preparando ventana principal...");
        m_loginAprobadoPendiente = true;
        abrirVentanaSiLista();
        return;
    }

    m_intentosFallidos++;
    Logger::log("Intento de login fallido. Intento numero: " + QString::number(m_intentosFallidos));

    if (m_intentosFallidos >= 3) {
        m_bloqueoHasta = QDateTime::currentDateTime().addSecs(30);
        m_intentosFallidos = 0;
        setLoginEnabled(false);
        m_lockTimer.start();
        ui->lblMensaje->setText("Demasiados intentos fallidos. Login bloqueado 30 segundos.");
        Logger::log("Login bloqueado temporalmente por multiples intentos fallidos.");
        return;
    }

    ui->lblMensaje->setText("Usuario o contrasena incorrecta.");
}

void Login::onClimaActualizado(const ClimaInfo &info)
{
    m_climaActual = info;

    ui->lblTemperatura->setText("Temperatura en " + info.ciudad + ": " + QString::number(info.temperatura, 'f', 1) + " C");
    ui->lblHora->setText("Hora local: " + info.horaLocal.toString("hh:mm:ss"));
    ui->lblEstadoRed->setText(info.offline ? "Modo offline" : "Online");
    ui->lblEstadoRed->setStyleSheet(info.offline ? "color: #A35A00;" : "color: #1D6A2B;");

    if (!m_clockTimer.isActive()) {
        m_clockTimer.start();
    }
}

void Login::onErrorAmigable(const QString &mensaje)
{
    ui->lblMensaje->setText(mensaje);
    Logger::log("Mensaje amigable UI: " + mensaje);
}

void Login::onLockTimerTick()
{
    if (QDateTime::currentDateTime() >= m_bloqueoHasta) {
        m_lockTimer.stop();
        setLoginEnabled(true);
        ui->lblMensaje->setText("Login habilitado nuevamente.");
        Logger::log("Bloqueo temporal finalizado.");
        return;
    }

    const int secs = QDateTime::currentDateTime().secsTo(m_bloqueoHasta);
    ui->lblMensaje->setText("Login bloqueado. Reintenta en " + QString::number(secs) + " segundos.");
}

void Login::onClockTick()
{
    if (!m_climaActual.horaLocal.isValid()) {
        return;
    }

    m_climaActual.horaLocal = m_climaActual.horaLocal.addSecs(1);
    ui->lblHora->setText("Hora local: " + m_climaActual.horaLocal.toString("hh:mm:ss"));
}

void Login::abrirVentanaSiLista()
{
    if (!m_loginAprobadoPendiente) {
        return;
    }

    if (!m_backgroundReady) {
        ui->lblMensaje->setText("Credenciales validas. Esperando descarga de imagen...");
        return;
    }

    if (!m_ventana) {
        m_ventana = new Ventana(m_backgroundCachePath, m_config.profilePhotoPath, m_config.profilePhotoUrl);
    }

    Logger::log("Abriendo ventana principal en pantalla completa.");
    m_ventana->showFullScreen();
    hide();
}

void Login::setLoginEnabled(bool enabled)
{
    ui->editUsuario->setEnabled(enabled);
    ui->editPassword->setEnabled(enabled);
    ui->btnIngresar->setEnabled(enabled);
}
