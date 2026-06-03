#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class QNetworkProxy;

struct ProxyConfig
{
    bool enabled = false;
    QString host;
    int port = 8080;
    QString user;
    QString password;
};

class AppConfig
{
public:
    explicit AppConfig(const QString &baseDir);

    void load();

    QString apiKey;
    QString city;
    QString units;
    QString weatherUrl;
    QString backgroundUrl;
    QString profilePhotoPath;
    QString profilePhotoUrl;
    ProxyConfig proxy;
    QString configPath;

private:
    QString m_baseDir;
    void ensureExists();
};

#endif // APPCONFIG_H
