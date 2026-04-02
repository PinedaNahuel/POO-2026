#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QWidget>

#include "monitorclient.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWindow(QWidget *parent = nullptr);

private slots:
    void refreshNow();
    void applyEndpointFromField();
    void applyInterval(int value);
    void applyThreshold(int value);
    void showLoadingState();
    void updateDashboard(const HealthSnapshot &snapshot);

private:
    QWidget *createMetricCard(const QString &title, const QString &caption, QLabel **valueLabel, QLabel **captionLabel);
    void applyStyles();
    QString formatUptime(qint64 seconds) const;
    QString formatPercent(double value) const;
    void applyStatusBadge(const QString &status);
    void updateMetricTone(QLabel *label, double value, bool higherIsWorse = true);

    MonitorClient m_monitor;
    QLineEdit *m_endpointEdit = nullptr;
    QSpinBox *m_intervalSpin = nullptr;
    QSpinBox *m_thresholdSpin = nullptr;
    QPushButton *m_refreshButton = nullptr;
    QLabel *m_statusBadge = nullptr;
    QLabel *m_statusSummary = nullptr;
    QLabel *m_lastCheckLabel = nullptr;
    QLabel *m_modeLabel = nullptr;
    QLabel *m_uptimeValue = nullptr;
    QLabel *m_cpuValue = nullptr;
    QLabel *m_memoryValue = nullptr;
    QLabel *m_diskValue = nullptr;
    QLabel *m_latencyValue = nullptr;
    QLabel *m_temperatureValue = nullptr;
    QLabel *m_uptimeCaption = nullptr;
    QLabel *m_cpuCaption = nullptr;
    QLabel *m_memoryCaption = nullptr;
    QLabel *m_diskCaption = nullptr;
    QLabel *m_latencyCaption = nullptr;
    QLabel *m_temperatureCaption = nullptr;
    QListWidget *m_historyList = nullptr;
};

#endif