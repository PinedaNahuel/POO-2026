#include "appconfig.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>

AppConfig::AppConfig(const QString &baseDir)
    : m_baseDir(baseDir)
{
    // Prioriza el config.ini de la raiz del proyecto (donde esta el .pro).
    QDir dir(m_baseDir);
    QString projectRootConfig;
    QString nearestConfig;

    for (int i = 0; i < 8; ++i) {
        const QStringList proFiles = dir.entryList(QStringList() << "*.pro", QDir::Files);
        if (!proFiles.isEmpty()) {
            const QString candidate = dir.filePath("config.ini");
            if (QFileInfo::exists(candidate)) {
                projectRootConfig = candidate;
                break;
            }
        }

        if (nearestConfig.isEmpty()) {
            const QString candidate = dir.filePath("config.ini");
            if (QFileInfo::exists(candidate)) {
                nearestConfig = candidate;
            }
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    if (!projectRootConfig.isEmpty()) {
        configPath = projectRootConfig;
    } else if (!nearestConfig.isEmpty()) {
        configPath = nearestConfig;
    } else {
        configPath = QDir(m_baseDir).filePath("config.ini");
    }
}

void AppConfig::ensureExists()
{
    QFile file(configPath);
    if (file.exists()) {
        return;
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "[api]\n";
        out << "weather_url=https://api.openweathermap.org/data/2.5/weather\n";
        out << "key=REEMPLAZAR_CON_TU_API_KEY\n";
        out << "city=Cordoba,AR\n";
        out << "units=metric\n\n";
        out << "[ui]\n";
        out << "background_url=https://picsum.photos/1920/1080\n";
        out << "profile_photo_path=\n";
        out << "profile_photo_url=https://randomuser.me/api/portraits/men/32.jpg\n\n";
        out << "[proxy]\n";
        out << "enabled=false\n";
        out << "host=\n";
        out << "port=8080\n";
        out << "user=\n";
        out << "password=\n";
    }
}

void AppConfig::load()
{
    ensureExists();

    QSettings settings(configPath, QSettings::IniFormat);
    const auto readIniString = [&settings](const QString &key, const QString &fallback) {
        const QVariant v = settings.value(key, fallback);
        if (v.metaType().id() == QMetaType::QStringList) {
            const QStringList list = v.toStringList();
            if (!list.isEmpty()) {
                return list.join(",");
            }
        }
        return v.toString();
    };

    weatherUrl = readIniString("api/weather_url", "https://api.openweathermap.org/data/2.5/weather");
    apiKey = readIniString("api/key", "");
    city = readIniString("api/city", "Cordoba,AR");
    units = readIniString("api/units", "metric");

    profilePhotoPath = readIniString("ui/profile_photo_path", "").trimmed();
    if (!profilePhotoPath.isEmpty()) {
        QFileInfo photoInfo(profilePhotoPath);
        if (photoInfo.isRelative()) {
            const QString configDir = QFileInfo(configPath).absolutePath();
            profilePhotoPath = QDir(configDir).filePath(profilePhotoPath);
        }
    }
    backgroundUrl = settings.value("ui/background_url", "https://picsum.photos/1920/1080").toString();
    profilePhotoUrl = settings.value("ui/profile_photo_url", "https://randomuser.me/api/portraits/men/32.jpg").toString();

    proxy.enabled = settings.value("proxy/enabled", false).toBool();
    proxy.host = settings.value("proxy/host", "").toString();
    proxy.port = settings.value("proxy/port", 8080).toInt();
    proxy.user = settings.value("proxy/user", "").toString();
    proxy.password = settings.value("proxy/password", "").toString();
}
