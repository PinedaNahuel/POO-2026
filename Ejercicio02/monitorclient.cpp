#include "monitorclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtMath>

namespace {

QString normalizedStatus(const QString &rawStatus)
{
    const QString value = rawStatus.trimmed().toLower();

    if (value == "ok" || value == "healthy" || value == "up") {
        return "OK";
    }

    if (value == "alert" || value == "warning" || value == "warn" || value == "degraded") {
        return "ALERTA";
    }

    if (value == "down" || value == "critical" || value == "offline" || value == "error") {
        return "CAIDO";
    }

    return "OK";
}

QString stringFromJson(const QJsonObject &root, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (value.isString()) {
            return value.toString();
        }
    }

    return QString();
}

double numberFromJson(const QJsonObject &root, const QStringList &keys, double fallback = 0.0)
{
    for (const QString &key : keys) {
        const QJsonValue value = root.value(key);
        if (value.isDouble()) {
            return value.toDouble();
        }

        if (value.isString()) {
            bool ok = false;
            const double parsed = value.toString().toDouble(&ok);
            if (ok) {
                return parsed;
            }
        }
    }

    return fallback;
}

QDateTime parseDateTime(const QString &rawDateTime)
{
    if (rawDateTime.isEmpty()) {
        return QDateTime();
    }

    QDateTime parsed = QDateTime::fromString(rawDateTime, Qt::ISODateWithMs);
    if (!parsed.isValid()) {
        parsed = QDateTime::fromString(rawDateTime, Qt::ISODate);
    }

    return parsed;
}

QString formatEventLabel(const QString &timeText, const QString &messageText)
{
    if (timeText.isEmpty()) {
        return messageText;
    }

    return timeText + " - " + messageText;
}

} // namespace

MonitorClient::MonitorClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_refreshTimer, &QTimer::timeout, this, &MonitorClient::performRefresh);
    connect(&m_networkManager, &QNetworkAccessManager::finished, this, &MonitorClient::handleReply);

    m_refreshTimer.setInterval(m_refreshIntervalSeconds * 1000);
    m_demoSessionStartedAt = QDateTime::currentDateTimeUtc();
    setEndpointUrl(QUrl(QStringLiteral("demo://vps-health")));
}

void MonitorClient::setEndpointUrl(const QUrl &url)
{
    if (url.isValid() && !url.isEmpty()) {
        m_endpointUrl = url;
        return;
    }

    m_endpointUrl = QUrl(QStringLiteral("demo://vps-health"));
}

QUrl MonitorClient::endpointUrl() const
{
    return m_endpointUrl;
}

void MonitorClient::setRefreshIntervalSeconds(int seconds)
{
    m_refreshIntervalSeconds = qMax(5, seconds);
    m_refreshTimer.setInterval(m_refreshIntervalSeconds * 1000);
}

int MonitorClient::refreshIntervalSeconds() const
{
    return m_refreshIntervalSeconds;
}

void MonitorClient::setAlertThresholdPercent(int threshold)
{
    m_alertThresholdPercent = qBound(50, threshold, 99);
}

int MonitorClient::alertThresholdPercent() const
{
    return m_alertThresholdPercent;
}

void MonitorClient::requestRefresh()
{
    performRefresh();
}

void MonitorClient::performRefresh()
{
    emit requestStarted();

    if (m_endpointUrl.scheme() == "demo") {
        QTimer::singleShot(250, this, [this]() {
            emit snapshotUpdated(buildDemoSnapshot());
        });

        if (!m_refreshTimer.isActive()) {
            m_refreshTimer.start();
        }
        return;
    }

    QNetworkRequest request(m_endpointUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Accept", "application/json");

    QNetworkReply *reply = m_networkManager.get(request);
    reply->setProperty("startedMs", QDateTime::currentMSecsSinceEpoch());

    if (!m_refreshTimer.isActive()) {
        m_refreshTimer.start();
    }
}

void MonitorClient::handleReply(QNetworkReply *reply)
{
    const qint64 startedMs = reply->property("startedMs").toLongLong();
    const double measuredLatencyMs = startedMs > 0
        ? static_cast<double>(QDateTime::currentMSecsSinceEpoch() - startedMs)
        : 0.0;

    if (reply->error() != QNetworkReply::NoError) {
        HealthSnapshot snapshot;
        snapshot.status = "CAIDO";
        snapshot.reachable = false;
        snapshot.lastError = reply->errorString();
        snapshot.checkedAt = QDateTime::currentDateTimeUtc();
        snapshot.latencyMs = measuredLatencyMs;
        snapshot.events << QStringLiteral("Sin respuesta del endpoint: %1").arg(reply->errorString());
        emit snapshotUpdated(decorateSnapshot(snapshot));
        reply->deleteLater();
        return;
    }

    QString errorText;
    const HealthSnapshot snapshot = parseSnapshot(reply->readAll(), measuredLatencyMs, &errorText);
    HealthSnapshot finalSnapshot = snapshot;
    if (!errorText.isEmpty()) {
        finalSnapshot.reachable = false;
        finalSnapshot.status = "CAIDO";
        finalSnapshot.lastError = errorText;
        finalSnapshot.checkedAt = QDateTime::currentDateTimeUtc();
        finalSnapshot.events.prepend(QStringLiteral("Respuesta invalida del endpoint: %1").arg(errorText));
    }

    emit snapshotUpdated(decorateSnapshot(finalSnapshot));
    reply->deleteLater();
}

HealthSnapshot MonitorClient::parseSnapshot(const QByteArray &payload, double measuredLatencyMs, QString *errorText) const
{
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        if (errorText) {
            *errorText = parseError.errorString();
        }
        return {};
    }

    if (!document.isObject()) {
        if (errorText) {
            *errorText = QStringLiteral("El JSON raiz debe ser un objeto.");
        }
        return {};
    }

    const QJsonObject root = document.object();
    const QJsonObject metrics = root.value("metrics").toObject();

    auto stringValue = [&](const QStringList &keys) {
        const QString rootValue = stringFromJson(root, keys);
        return rootValue.isEmpty() ? stringFromJson(metrics, keys) : rootValue;
    };

    auto numberValue = [&](const QStringList &keys, double fallback = 0.0) {
        const double rootValue = numberFromJson(root, keys, fallback);
        if (rootValue != fallback) {
            return rootValue;
        }
        return numberFromJson(metrics, keys, fallback);
    };

    HealthSnapshot snapshot;
    snapshot.status = normalizedStatus(stringValue({"status", "overall_status"}));
    snapshot.uptimeSeconds = static_cast<qint64>(numberValue({"uptime_seconds", "uptime_sec", "uptime"}, 0.0));
    snapshot.cpuPercent = numberValue({"cpu_percent", "cpu_usage", "cpu_load"}, 0.0);
    snapshot.memoryPercent = numberValue({"memory_percent", "ram_percent", "memory_usage"}, 0.0);
    snapshot.diskPercent = numberValue({"disk_percent", "disk_usage", "storage_percent"}, 0.0);
    snapshot.latencyMs = numberValue({"latency_ms", "response_ms"}, measuredLatencyMs);
    snapshot.temperatureC = numberValue({"temperature_c", "temp_c", "cpu_temp_c"}, -1.0);
    snapshot.reachable = true;
    snapshot.checkedAt = parseDateTime(stringValue({"checked_at", "timestamp", "last_check"}));
    if (!snapshot.checkedAt.isValid()) {
        snapshot.checkedAt = QDateTime::currentDateTimeUtc();
    }

    const QJsonArray eventsArray = root.value("events").toArray();
    for (const QJsonValue &value : eventsArray) {
        if (value.isString()) {
            snapshot.events << value.toString();
            continue;
        }

        if (!value.isObject()) {
            continue;
        }

        const QJsonObject eventObject = value.toObject();
        snapshot.events << formatEventLabel(
            stringFromJson(eventObject, {"time", "timestamp"}),
            stringFromJson(eventObject, {"message", "text", "event"}));
    }

    return snapshot;
}

HealthSnapshot MonitorClient::decorateSnapshot(HealthSnapshot snapshot)
{
    if (!snapshot.checkedAt.isValid()) {
        snapshot.checkedAt = QDateTime::currentDateTimeUtc();
    }

    if (m_hasLastSnapshot && snapshot.reachable && m_lastSnapshot.reachable) {
        const qint64 elapsedSinceLast = qMax<qint64>(0, m_lastSnapshot.checkedAt.secsTo(snapshot.checkedAt));

        if (snapshot.uptimeSeconds <= 0 && m_lastSnapshot.uptimeSeconds > 0) {
            snapshot.uptimeSeconds = m_lastSnapshot.uptimeSeconds + elapsedSinceLast;
        }

        // If uptime goes backwards while service is still reachable, keep continuity.
        if (snapshot.uptimeSeconds > 0
            && m_lastSnapshot.uptimeSeconds > 0
            && snapshot.uptimeSeconds + 60 < m_lastSnapshot.uptimeSeconds
            && snapshot.status != "CAIDO") {
            snapshot.events.prepend(QStringLiteral("Uptime inconsistente recibido. Se conserva continuidad local."));
            snapshot.uptimeSeconds = m_lastSnapshot.uptimeSeconds + elapsedSinceLast;
        }
    }

    if (!snapshot.reachable) {
        snapshot.status = QStringLiteral("CAIDO");
    } else if (snapshot.status.isEmpty()) {
        snapshot.status = QStringLiteral("OK");
    }

    const bool thresholdExceeded = snapshot.cpuPercent >= m_alertThresholdPercent
        || snapshot.memoryPercent >= m_alertThresholdPercent
        || snapshot.diskPercent >= m_alertThresholdPercent
        || snapshot.latencyMs >= 1000.0;

    if (snapshot.status == "OK" && thresholdExceeded) {
        snapshot.status = QStringLiteral("ALERTA");
    }

    if (snapshot.events.isEmpty()) {
        snapshot.events = buildDerivedEvents(snapshot);
    }

    if (!snapshot.lastError.isEmpty() && snapshot.events.isEmpty()) {
        snapshot.events << snapshot.lastError;
    }

    while (snapshot.events.size() > 8) {
        snapshot.events.removeLast();
    }

    m_lastSnapshot = snapshot;
    m_hasLastSnapshot = true;

    return snapshot;
}

HealthSnapshot MonitorClient::buildDemoSnapshot()
{
    ++m_demoTick;

    if (!m_demoInitialized) {
        m_demoInitialized = true;
        m_demoSessionStartedAt = QDateTime::currentDateTimeUtc();
    }

    HealthSnapshot snapshot;
    snapshot.reachable = true;

    const qint64 elapsedDemoSeconds = qMax<qint64>(0, m_demoSessionStartedAt.secsTo(QDateTime::currentDateTimeUtc()));
    snapshot.uptimeSeconds = m_demoBaseUptimeSeconds + elapsedDemoSeconds;

    const double phase = static_cast<double>(m_demoTick) / 6.0;
    const double cpuTarget = 36.0 + 10.0 * qSin(phase);
    const double memoryTarget = 61.0 + 6.0 * qSin(phase * 0.7 + 0.6);
    const double diskTarget = 67.0 + 2.0 * qSin(phase * 0.2);
    const double latencyTarget = 105.0 + 30.0 * qSin(phase * 1.2);
    const double tempTarget = 52.0 + 3.0 * qSin(phase * 0.8 + 0.3);

    m_demoCpuPercent += (cpuTarget - m_demoCpuPercent) * 0.35;
    m_demoMemoryPercent += (memoryTarget - m_demoMemoryPercent) * 0.30;
    m_demoDiskPercent += (diskTarget - m_demoDiskPercent) * 0.15;
    m_demoLatencyMs += (latencyTarget - m_demoLatencyMs) * 0.45;
    m_demoTemperatureC += (tempTarget - m_demoTemperatureC) * 0.30;

    snapshot.cpuPercent = qBound(8.0, m_demoCpuPercent, 96.0);
    snapshot.memoryPercent = qBound(20.0, m_demoMemoryPercent, 92.0);
    snapshot.diskPercent = qBound(45.0, m_demoDiskPercent, 90.0);
    snapshot.latencyMs = qBound(25.0, m_demoLatencyMs, 900.0);
    snapshot.temperatureC = qBound(35.0, m_demoTemperatureC, 80.0);
    snapshot.checkedAt = QDateTime::currentDateTimeUtc();

    if (m_demoTick % 10 == 0) {
        snapshot.status = QStringLiteral("ALERTA");
        snapshot.events << QStringLiteral("Pico controlado de CPU detectado en muestra de demostracion.");
        snapshot.cpuPercent = qMax(snapshot.cpuPercent, static_cast<double>(m_alertThresholdPercent + 2));
    } else {
        snapshot.status = QStringLiteral("OK");
        snapshot.events << QStringLiteral("Agente demo operativo con tendencia estable entre chequeos.");
    }

    snapshot.events << QStringLiteral("Chequeo completado contra el origen de datos demo interno.");

    return decorateSnapshot(snapshot);
}

QStringList MonitorClient::buildDerivedEvents(const HealthSnapshot &snapshot) const
{
    QStringList events;

    if (!snapshot.reachable) {
        events << QStringLiteral("Endpoint inaccesible.");
        return events;
    }

    if (snapshot.status == "OK") {
        events << QStringLiteral("Todas las metricas se encuentran dentro del rango esperado.");
    }

    if (snapshot.cpuPercent >= m_alertThresholdPercent) {
        events << QStringLiteral("CPU por encima del umbral configurado (%1%).").arg(m_alertThresholdPercent);
    }

    if (snapshot.memoryPercent >= m_alertThresholdPercent) {
        events << QStringLiteral("Memoria por encima del umbral configurado (%1%).").arg(m_alertThresholdPercent);
    }

    if (snapshot.diskPercent >= m_alertThresholdPercent) {
        events << QStringLiteral("Disco por encima del umbral configurado (%1%).").arg(m_alertThresholdPercent);
    }

    if (snapshot.latencyMs >= 1000.0) {
        events << QStringLiteral("Latencia mayor a 1000 ms.");
    }

    if (snapshot.temperatureC >= 75.0) {
        events << QStringLiteral("Temperatura alta en el host.");
    }

    return events;
}