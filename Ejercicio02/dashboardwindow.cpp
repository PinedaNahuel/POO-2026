#include "dashboardwindow.h"

#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Cloud VPS Monitor");
    resize(1180, 760);
    setMinimumSize(980, 680);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(26, 22, 26, 22);
    rootLayout->setSpacing(18);

    QLabel *titleLabel = new QLabel("Cloud VPS Monitor", this);
    titleLabel->setObjectName("pageTitle");

    QLabel *subtitleLabel = new QLabel(
        "Panel de salud para un VPS propio con chequeos HTTP, alertas basicas e historial corto de eventos.",
        this);
    subtitleLabel->setObjectName("pageSubtitle");
    subtitleLabel->setWordWrap(true);

    QFrame *controlsFrame = new QFrame(this);
    controlsFrame->setObjectName("controlsFrame");
    QGridLayout *controlsLayout = new QGridLayout(controlsFrame);
    controlsLayout->setContentsMargins(18, 18, 18, 18);
    controlsLayout->setHorizontalSpacing(14);
    controlsLayout->setVerticalSpacing(12);

    QLabel *endpointLabel = new QLabel("Endpoint", controlsFrame);
    endpointLabel->setObjectName("fieldLabel");
    m_endpointEdit = new QLineEdit(QStringLiteral("demo://vps-health"), controlsFrame);
    m_endpointEdit->setPlaceholderText("http://tu-vps:8080/health o demo://vps-health");

    QLabel *intervalLabel = new QLabel("Intervalo (s)", controlsFrame);
    intervalLabel->setObjectName("fieldLabel");
    m_intervalSpin = new QSpinBox(controlsFrame);
    m_intervalSpin->setRange(5, 300);
    m_intervalSpin->setValue(15);

    QLabel *thresholdLabel = new QLabel("Umbral alerta (%)", controlsFrame);
    thresholdLabel->setObjectName("fieldLabel");
    m_thresholdSpin = new QSpinBox(controlsFrame);
    m_thresholdSpin->setRange(50, 99);
    m_thresholdSpin->setValue(85);

    m_refreshButton = new QPushButton("Refrescar ahora", controlsFrame);
    m_refreshButton->setObjectName("refreshButton");

    m_modeLabel = new QLabel("Modo demo listo para reemplazar por la URL real.", controlsFrame);
    m_modeLabel->setObjectName("modeLabel");
    m_modeLabel->setWordWrap(true);

    controlsLayout->addWidget(endpointLabel, 0, 0);
    controlsLayout->addWidget(m_endpointEdit, 0, 1, 1, 3);
    controlsLayout->addWidget(intervalLabel, 1, 0);
    controlsLayout->addWidget(m_intervalSpin, 1, 1);
    controlsLayout->addWidget(thresholdLabel, 1, 2);
    controlsLayout->addWidget(m_thresholdSpin, 1, 3);
    controlsLayout->addWidget(m_refreshButton, 1, 4);
    controlsLayout->addWidget(m_modeLabel, 2, 0, 1, 5);
    controlsLayout->setColumnStretch(1, 1);

    QFrame *statusFrame = new QFrame(this);
    statusFrame->setObjectName("statusFrame");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusFrame);
    statusLayout->setContentsMargins(18, 16, 18, 16);
    statusLayout->setSpacing(14);

    m_statusBadge = new QLabel("SIN DATOS", statusFrame);
    m_statusBadge->setObjectName("statusBadge");
    m_statusBadge->setAlignment(Qt::AlignCenter);

    m_statusSummary = new QLabel("Esperando primer chequeo.", statusFrame);
    m_statusSummary->setObjectName("statusSummary");
    m_statusSummary->setWordWrap(true);

    m_lastCheckLabel = new QLabel("Ultimo chequeo: pendiente", statusFrame);
    m_lastCheckLabel->setObjectName("lastCheckLabel");
    m_lastCheckLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    statusLayout->addWidget(m_statusBadge, 0);
    statusLayout->addWidget(m_statusSummary, 1);
    statusLayout->addWidget(m_lastCheckLabel, 0);

    QGridLayout *metricsLayout = new QGridLayout;
    metricsLayout->setHorizontalSpacing(16);
    metricsLayout->setVerticalSpacing(16);

    metricsLayout->addWidget(createMetricCard("Uptime", "Disponibilidad continua del host", &m_uptimeValue, &m_uptimeCaption), 0, 0);
    metricsLayout->addWidget(createMetricCard("CPU", "Carga actual del servidor", &m_cpuValue, &m_cpuCaption), 0, 1);
    metricsLayout->addWidget(createMetricCard("Memoria", "Uso total de RAM", &m_memoryValue, &m_memoryCaption), 0, 2);
    metricsLayout->addWidget(createMetricCard("Disco", "Capacidad consumida", &m_diskValue, &m_diskCaption), 1, 0);
    metricsLayout->addWidget(createMetricCard("Latencia", "Tiempo de respuesta del endpoint", &m_latencyValue, &m_latencyCaption), 1, 1);
    metricsLayout->addWidget(createMetricCard("Temperatura", "Lectura opcional del host", &m_temperatureValue, &m_temperatureCaption), 1, 2);

    QFrame *historyFrame = new QFrame(this);
    historyFrame->setObjectName("historyFrame");
    QVBoxLayout *historyLayout = new QVBoxLayout(historyFrame);
    historyLayout->setContentsMargins(18, 18, 18, 18);
    historyLayout->setSpacing(10);

    QLabel *historyTitle = new QLabel("Historial corto de eventos", historyFrame);
    historyTitle->setObjectName("sectionTitle");

    QLabel *historySubtitle = new QLabel(
        "Muestra las ultimas incidencias o anotaciones derivadas del endpoint y de la evaluacion local del panel.",
        historyFrame);
    historySubtitle->setObjectName("sectionSubtitle");
    historySubtitle->setWordWrap(true);

    m_historyList = new QListWidget(historyFrame);
    m_historyList->setSelectionMode(QAbstractItemView::NoSelection);
    m_historyList->setFocusPolicy(Qt::NoFocus);
    m_historyList->setAlternatingRowColors(true);

    historyLayout->addWidget(historyTitle);
    historyLayout->addWidget(historySubtitle);
    historyLayout->addWidget(m_historyList, 1);

    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(subtitleLabel);
    rootLayout->addWidget(controlsFrame);
    rootLayout->addWidget(statusFrame);
    rootLayout->addLayout(metricsLayout);
    rootLayout->addWidget(historyFrame, 1);

    applyStyles();

    connect(m_refreshButton, &QPushButton::clicked, this, &DashboardWindow::refreshNow);
    connect(m_endpointEdit, &QLineEdit::editingFinished, this, &DashboardWindow::applyEndpointFromField);
    connect(m_intervalSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &DashboardWindow::applyInterval);
    connect(m_thresholdSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &DashboardWindow::applyThreshold);
    connect(&m_monitor, &MonitorClient::requestStarted, this, &DashboardWindow::showLoadingState);
    connect(&m_monitor, &MonitorClient::snapshotUpdated, this, &DashboardWindow::updateDashboard);

    m_monitor.setRefreshIntervalSeconds(m_intervalSpin->value());
    m_monitor.setAlertThresholdPercent(m_thresholdSpin->value());
    refreshNow();
}

QWidget *DashboardWindow::createMetricCard(const QString &title, const QString &caption, QLabel **valueLabel, QLabel **captionLabel)
{
    QFrame *card = new QFrame(this);
    card->setObjectName("metricCard");
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    QLabel *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("metricTitle");

    QLabel *value = new QLabel("--", card);
    value->setObjectName("metricValue");

    QLabel *captionText = new QLabel(caption, card);
    captionText->setObjectName("metricCaption");
    captionText->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(value);
    layout->addWidget(captionText);
    layout->addStretch();

    *valueLabel = value;
    *captionLabel = captionText;

    return card;
}

void DashboardWindow::applyStyles()
{
    setStyleSheet(
        "QWidget {"
        "  background: #f4f6f8;"
        "  color: #132238;"
        "  font-family: 'Segoe UI';"
        "}"
        "QFrame#controlsFrame, QFrame#statusFrame, QFrame#historyFrame, QFrame#metricCard {"
        "  background: #ffffff;"
        "  border: 1px solid #d8e0e7;"
        "  border-radius: 16px;"
        "}"
        "QLabel#pageTitle {"
        "  font-size: 28px;"
        "  font-weight: 700;"
        "  color: #10243d;"
        "}"
        "QLabel#pageSubtitle, QLabel#sectionSubtitle, QLabel#modeLabel, QLabel#metricCaption {"
        "  color: #5f6f82;"
        "  font-size: 13px;"
        "}"
        "QLabel#fieldLabel, QLabel#sectionTitle, QLabel#metricTitle {"
        "  color: #24384f;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "  text-transform: uppercase;"
        "}"
        "QLabel#statusBadge {"
        "  min-width: 128px;"
        "  padding: 10px 18px;"
        "  border-radius: 12px;"
        "  font-size: 14px;"
        "  font-weight: 700;"
        "  background: #dce3ea;"
        "  color: #445667;"
        "}"
        "QLabel#statusSummary {"
        "  font-size: 15px;"
        "  font-weight: 600;"
        "}"
        "QLabel#lastCheckLabel {"
        "  color: #41556d;"
        "  font-size: 13px;"
        "}"
        "QLabel#metricValue {"
        "  font-size: 34px;"
        "  font-weight: 700;"
        "  color: #10243d;"
        "}"
        "QLineEdit, QSpinBox, QListWidget {"
        "  background: #fbfcfd;"
        "  border: 1px solid #ccd6df;"
        "  border-radius: 10px;"
        "  padding: 8px 10px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus {"
        "  border: 1px solid #2e7d6e;"
        "}"
        "QPushButton#refreshButton {"
        "  background: #173f5f;"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 10px;"
        "  padding: 10px 16px;"
        "  font-weight: 600;"
        "}"
        "QPushButton#refreshButton:hover {"
        "  background: #22587f;"
        "}"
        "QPushButton#refreshButton:pressed {"
        "  background: #102f46;"
        "}"
        "QListWidget {"
        "  padding: 6px;"
        "}"
        "QListWidget::item {"
        "  border-bottom: 1px solid #edf1f4;"
        "  padding: 10px 8px;"
        "}"
        "QListWidget::item:alternate {"
        "  background: #f8fafb;"
        "}"
    );
}

void DashboardWindow::refreshNow()
{
    applyEndpointFromField();
    m_monitor.requestRefresh();
}

void DashboardWindow::applyEndpointFromField()
{
    const QString rawText = m_endpointEdit->text().trimmed();
    if (rawText.isEmpty()) {
        m_endpointEdit->setText(QStringLiteral("demo://vps-health"));
    }

    m_monitor.setEndpointUrl(QUrl(m_endpointEdit->text().trimmed()));

    if (m_monitor.endpointUrl().scheme() == "demo") {
        m_modeLabel->setText("Modo demo activo. Cambia la URL por tu endpoint HTTP real cuando lo tengas.");
    } else {
        m_modeLabel->setText(QStringLiteral("Origen activo: %1").arg(m_monitor.endpointUrl().toString()));
    }
}

void DashboardWindow::applyInterval(int value)
{
    m_monitor.setRefreshIntervalSeconds(value);
}

void DashboardWindow::applyThreshold(int value)
{
    m_monitor.setAlertThresholdPercent(value);
}

void DashboardWindow::showLoadingState()
{
    m_statusSummary->setText("Consultando estado del servidor...");
    m_lastCheckLabel->setText("Ultimo chequeo: actualizando");
}

void DashboardWindow::updateDashboard(const HealthSnapshot &snapshot)
{
    applyStatusBadge(snapshot.status);

    if (snapshot.status == "OK") {
        m_statusSummary->setText("Servidor operativo. No se detectan condiciones criticas en esta muestra.");
    } else if (snapshot.status == "ALERTA") {
        m_statusSummary->setText("El servidor responde, pero una o mas metricas quedaron fuera del rango recomendado.");
    } else {
        m_statusSummary->setText("El endpoint no pudo ser consultado o reporta una caida del servicio.");
    }

    const QDateTime localTime = snapshot.checkedAt.toLocalTime();
    m_lastCheckLabel->setText(QStringLiteral("Ultimo chequeo: %1")
                                  .arg(localTime.toString("dd/MM/yyyy HH:mm:ss")));

    m_uptimeValue->setText(formatUptime(snapshot.uptimeSeconds));
    m_cpuValue->setText(formatPercent(snapshot.cpuPercent));
    m_memoryValue->setText(formatPercent(snapshot.memoryPercent));
    m_diskValue->setText(formatPercent(snapshot.diskPercent));
    m_latencyValue->setText(QStringLiteral("%1 ms").arg(QString::number(snapshot.latencyMs, 'f', 0)));
    m_temperatureValue->setText(snapshot.temperatureC >= 0.0
        ? QStringLiteral("%1 C").arg(QString::number(snapshot.temperatureC, 'f', 1))
        : QStringLiteral("N/D"));

    m_uptimeCaption->setText("Tiempo continuo en linea del host.");
    m_cpuCaption->setText(QStringLiteral("Alerta desde %1%." ).arg(m_thresholdSpin->value()));
    m_memoryCaption->setText(QStringLiteral("Alerta desde %1%." ).arg(m_thresholdSpin->value()));
    m_diskCaption->setText(QStringLiteral("Alerta desde %1%." ).arg(m_thresholdSpin->value()));
    m_latencyCaption->setText("Se considera critica por encima de 1000 ms.");
    m_temperatureCaption->setText("Se muestra si el endpoint la informa.");

    updateMetricTone(m_cpuValue, snapshot.cpuPercent);
    updateMetricTone(m_memoryValue, snapshot.memoryPercent);
    updateMetricTone(m_diskValue, snapshot.diskPercent);
    updateMetricTone(m_latencyValue, snapshot.latencyMs >= 1000.0 ? 100.0 : 0.0);
    updateMetricTone(m_temperatureValue, snapshot.temperatureC >= 75.0 ? 100.0 : 0.0);
    m_uptimeValue->setStyleSheet("color: #10243d;");

    m_historyList->clear();
    for (const QString &eventText : snapshot.events) {
        m_historyList->addItem(eventText);
    }

    if (!snapshot.lastError.isEmpty()) {
        m_historyList->insertItem(0, QStringLiteral("Detalle tecnico: %1").arg(snapshot.lastError));
    }
}

QString DashboardWindow::formatUptime(qint64 seconds) const
{
    const qint64 days = seconds / 86400;
    seconds %= 86400;
    const qint64 hours = seconds / 3600;
    seconds %= 3600;
    const qint64 minutes = seconds / 60;

    return QStringLiteral("%1d %2h %3m").arg(days).arg(hours).arg(minutes);
}

QString DashboardWindow::formatPercent(double value) const
{
    return QStringLiteral("%1%").arg(QString::number(value, 'f', 1));
}

void DashboardWindow::applyStatusBadge(const QString &status)
{
    m_statusBadge->setText(status);

    if (status == "OK") {
        m_statusBadge->setStyleSheet("background: #dff4ea; color: #1f6b46;");
        return;
    }

    if (status == "ALERTA") {
        m_statusBadge->setStyleSheet("background: #fff0d8; color: #9b5a00;");
        return;
    }

    m_statusBadge->setStyleSheet("background: #f8dede; color: #a33434;");
}

void DashboardWindow::updateMetricTone(QLabel *label, double value, bool higherIsWorse)
{
    Q_UNUSED(higherIsWorse)

    if (value >= 100.0 || value >= m_thresholdSpin->value()) {
        label->setStyleSheet("color: #b24a2f;");
        return;
    }

    label->setStyleSheet("color: #10243d;");
}