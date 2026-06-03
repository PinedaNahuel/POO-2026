#ifndef CLIMASERVICIO_H
#define CLIMASERVICIO_H

#include "appconfig.h"
#include "datosbase.h"

#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkInformation>

struct ClimaInfo
{
    QString ciudad;
    double temperatura = 0.0;
    QDateTime horaLocal;
    bool offline = false;
};

class ClimaServicio : public DatosBase
{
    Q_OBJECT

public:
    explicit ClimaServicio(const AppConfig &config, QObject *parent = nullptr);
    void actualizar() override;

signals:
    void climaActualizado(const ClimaInfo &info);
    void errorAmigable(const QString &mensaje);
    void estadoConectividad(bool online, const QString &detalle);

private slots:
    void onReplyFinished();

private:
    AppConfig m_config;
    QNetworkAccessManager m_manager;
    bool hayConectividadProactiva() const;
    QString buildFriendlyError(QNetworkReply *reply) const;
    ClimaInfo simulatedData(const QString &motivo) const;
};

#endif // CLIMASERVICIO_H
