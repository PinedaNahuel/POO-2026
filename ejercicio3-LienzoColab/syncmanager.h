#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include "canvasmodel.h"

class SyncManager : public QObject {
    Q_OBJECT
public:
    explicit SyncManager(CanvasModel *model, QObject *parent = nullptr);

    void setServerUrl(const QUrl &url);
    void saveToServer();
    void loadFromServer();

    void startAutoSync(int intervalMs = 1500);
    void stopAutoSync();

signals:
    void syncError(const QString &error);
    void synced();

private slots:
    void onLoadFinished(QNetworkReply *reply);
    void onSaveFinished(QNetworkReply *reply);
    void onAutoSync();

private:
    CanvasModel *m_model;
    QNetworkAccessManager m_nam;
    QUrl m_serverUrl;
    QTimer m_timer;
    bool m_hasLocalChanges = false;
};

#endif // SYNCMANAGER_H