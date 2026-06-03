#include "climaservicio.h"

#include "logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QUrlQuery>

ClimaServicio::ClimaServicio(const AppConfig &config, QObject *parent)
    : DatosBase(parent)
    , m_config(config)
{
    if (m_config.proxy.enabled && !m_config.proxy.host.isEmpty()) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_config.proxy.host, m_config.proxy.port,
                            m_config.proxy.user, m_config.proxy.password);
        m_manager.setProxy(proxy);
        Logger::log("Proxy de clima habilitado: " + m_config.proxy.host + ":" + QString::number(m_config.proxy.port));
    }
}

void ClimaServicio::actualizar()
{
    if (m_config.apiKey.trimmed().isEmpty() || m_config.apiKey == "REEMPLAZAR_CON_TU_API_KEY") {
        Logger::log("Sin API key valida. Se usa modo offline para clima.");
        emit estadoConectividad(false, "Sin API key. Mostrando clima simulado.");
        emit errorAmigable("No hay API key configurada. Mostrando clima simulado.");
        emit climaActualizado(simulatedData("api-key-no-configurada"));
        return;
    }

    if (!hayConectividadProactiva()) {
        Logger::log("Sin conectividad detectada de forma proactiva. Se usa modo offline.");
        emit estadoConectividad(false, "Sin internet detectada. Mostrando clima simulado.");
        emit errorAmigable("Sin internet detectada. Mostrando clima simulado.");
        emit climaActualizado(simulatedData("offline-proactivo"));
        return;
    }

    emit estadoConectividad(true, "Conectividad disponible. Consultando API de clima.");

    QUrl url(m_config.weatherUrl);
    QUrlQuery query;
    query.addQueryItem("q", m_config.city);
    query.addQueryItem("units", m_config.units);
    query.addQueryItem("appid", m_config.apiKey);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtLoginClima/1.0");

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &ClimaServicio::onReplyFinished);
    Logger::log("Solicitud de clima enviada para ciudad: " + m_config.city);
}

void ClimaServicio::onReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString error = reply->errorString();
        const QVariant statusVar = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const int status = statusVar.isValid() ? statusVar.toInt() : 0;
        Logger::log("Error de red al consultar clima. HTTP=" + QString::number(status) + " Detalle=" + error);
        emit estadoConectividad(false, "Fallo de red al consultar API. Mostrando clima simulado.");
        emit errorAmigable(buildFriendlyError(reply));
        emit climaActualizado(simulatedData(error));
        reply->deleteLater();
        return;
    }

    const QByteArray payload = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        Logger::log("JSON de clima invalido: " + parseError.errorString());
        emit errorAmigable("La respuesta del clima no es valida. Usando modo offline.");
        emit climaActualizado(simulatedData("json-invalido"));
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    if (!root.contains("main") || !root.value("main").isObject()) {
        Logger::log("Respuesta de clima sin campo main.");
        emit errorAmigable("Faltan datos del clima. Usando modo offline.");
        emit climaActualizado(simulatedData("sin-main"));
        reply->deleteLater();
        return;
    }

    QJsonObject main = root.value("main").toObject();
    if (!main.contains("temp") || !main.value("temp").isDouble()) {
        Logger::log("Respuesta de clima sin temperatura valida.");
        emit errorAmigable("Temperatura no disponible. Usando modo offline.");
        emit climaActualizado(simulatedData("sin-temp"));
        reply->deleteLater();
        return;
    }

    const int timezoneOffset = root.value("timezone").toInt(0);
    const qint64 dt = root.value("dt").toInteger(QDateTime::currentSecsSinceEpoch());

    ClimaInfo info;
    info.ciudad = root.value("name").toString(m_config.city);
    info.temperatura = main.value("temp").toDouble();
    info.horaLocal = QDateTime::fromSecsSinceEpoch(dt + timezoneOffset, Qt::UTC);
    info.offline = false;

    Logger::log("Clima actualizado correctamente. Ciudad: " + info.ciudad);
    emit estadoConectividad(true, "Conexion estable con API de clima.");
    emit climaActualizado(info);
    reply->deleteLater();
}

bool ClimaServicio::hayConectividadProactiva() const
{
    if (!QNetworkInformation::loadDefaultBackend()) {
        // Si el backend no esta disponible, mantenemos deteccion reactiva.
        return true;
    }

    QNetworkInformation *netInfo = QNetworkInformation::instance();
    if (!netInfo) {
        return true;
    }

    return netInfo->reachability() != QNetworkInformation::Reachability::Disconnected;
}

QString ClimaServicio::buildFriendlyError(QNetworkReply *reply) const
{
    const QVariant statusVar = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    const int status = statusVar.isValid() ? statusVar.toInt() : 0;

    if (status == 401) {
        return "API key invalida o no activada todavia. Mostrando clima simulado.";
    }
    if (status == 404) {
        return "Ciudad no encontrada en la API. Revisa city en config.ini.";
    }
    if (status == 429) {
        return "Se alcanzo el limite de consultas de la API. Intenta mas tarde.";
    }
    if (status >= 500 && status < 600) {
        return "El servicio de clima no esta disponible ahora. Mostrando datos simulados.";
    }

    switch (reply->error()) {
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TimeoutError:
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::NetworkSessionFailedError:
        return "Sin conexion a internet o red bloqueada. Mostrando clima simulado.";
    case QNetworkReply::ProxyAuthenticationRequiredError:
        return "El proxy requiere autenticacion. Revisa user/password del proxy.";
    case QNetworkReply::ProxyNotFoundError:
    case QNetworkReply::ProxyConnectionRefusedError:
    case QNetworkReply::ProxyTimeoutError:
        return "No se pudo usar el proxy configurado. Revisa host y puerto.";
    case QNetworkReply::SslHandshakeFailedError:
        return "Fallo SSL/TLS al conectar con la API de clima.";
    default:
        break;
    }

    return "No se pudo obtener clima real ahora. Mostrando datos simulados.";
}

ClimaInfo ClimaServicio::simulatedData(const QString &motivo) const
{
    Q_UNUSED(motivo)

    ClimaInfo info;
    info.ciudad = m_config.city;
    info.temperatura = 22.0;
    info.horaLocal = QDateTime::currentDateTime();
    info.offline = true;
    return info;
}
