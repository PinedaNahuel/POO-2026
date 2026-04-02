#ifndef MONITORCLIENT_H
#define MONITORCLIENT_H

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>
#include <QUrl>

struct HealthSnapshot {
    QString status;
    qint64 uptimeSeconds = 0;
    double cpuPercent = 0.0;
    double memoryPercent = 0.0;
    double diskPercent = 0.0;
    double latencyMs = 0.0;
    double temperatureC = -1.0;
    bool reachable = false;
    QString lastError;
    QDateTime checkedAt;
    QStringList events;
};

class QNetworkReply;

class MonitorClient : public QObject
{
    Q_OBJECT

public:
    explicit MonitorClient(QObject *parent = nullptr);

    void setEndpointUrl(const QUrl &url);
    QUrl endpointUrl() const;

    void setRefreshIntervalSeconds(int seconds);
    int refreshIntervalSeconds() const;

    void setAlertThresholdPercent(int threshold);
    int alertThresholdPercent() const;

public slots:
    void requestRefresh();

signals:
    void requestStarted();
    void snapshotUpdated(const HealthSnapshot &snapshot);

private slots:
    void performRefresh();
    void handleReply(QNetworkReply *reply);

private:
    HealthSnapshot parseSnapshot(const QByteArray &payload, double measuredLatencyMs, QString *errorText) const;
    HealthSnapshot decorateSnapshot(HealthSnapshot snapshot);
    HealthSnapshot buildDemoSnapshot();
    QStringList buildDerivedEvents(const HealthSnapshot &snapshot) const;

    QNetworkAccessManager m_networkManager;
    QTimer m_refreshTimer;
    QUrl m_endpointUrl;
    int m_refreshIntervalSeconds = 15;
    int m_alertThresholdPercent = 85;
    int m_demoTick = 0;
    bool m_demoInitialized = false;
    qint64 m_demoBaseUptimeSeconds = 21 * 86400;
    QDateTime m_demoSessionStartedAt;
    double m_demoCpuPercent = 31.0;
    double m_demoMemoryPercent = 58.0;
    double m_demoDiskPercent = 62.0;
    double m_demoLatencyMs = 95.0;
    double m_demoTemperatureC = 50.0;
    bool m_hasLastSnapshot = false;
    HealthSnapshot m_lastSnapshot;
};

#endif