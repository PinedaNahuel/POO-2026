#include "syncmanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

SyncManager::SyncManager(CanvasModel *model, QObject *parent)
    : QObject(parent), m_model(model) {
    connect(&m_nam, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply){
        QString endpoint = reply->request().url().path();
        if (endpoint.endsWith("/load")) onLoadFinished(reply);
        else onSaveFinished(reply);
        reply->deleteLater();
    });

    connect(&m_timer, &QTimer::timeout, this, &SyncManager::onAutoSync);
    connect(m_model, &CanvasModel::localModelChanged, this, [this]() {
        m_hasLocalChanges = true;
    });
}

void SyncManager::setServerUrl(const QUrl &url) {
    m_serverUrl = url;
}

void SyncManager::saveToServer() {
    if (!m_serverUrl.isValid()) {
        emit syncError("URL de servidor inválida");
        return;
    }

    QUrl url = m_serverUrl;
    url.setPath(url.path() + "/save");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload = m_model->toJson();
    QNetworkReply *reply = m_nam.post(req, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError e){
        Q_UNUSED(e)
        emit syncError("Error en guardado de servidor");
    });
}

void SyncManager::loadFromServer() {
    if (!m_serverUrl.isValid()) {
        emit syncError("URL de servidor inválida");
        return;
    }

    QUrl url = m_serverUrl;
    url.setPath(url.path() + "/load");
    QNetworkRequest req(url);
    QNetworkReply *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError e){
        Q_UNUSED(e)
        emit syncError("Error en carga de servidor");
    });
}

void SyncManager::startAutoSync(int intervalMs) {
    m_timer.start(intervalMs);
}

void SyncManager::stopAutoSync() {
    m_timer.stop();
}

void SyncManager::onAutoSync() {
    if (m_hasLocalChanges) {
        return;
    }

    loadFromServer();
}

void SyncManager::onLoadFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) return;
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        emit syncError("Respuesta servidor incorrectly formatted");
        return;
    }

    if (!m_hasLocalChanges && m_model->fromJson(doc.object())) {
        emit synced();
    }
}

void SyncManager::onSaveFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit syncError("Error en respuesta de guardar");
        return;
    }

    m_hasLocalChanges = false;
    emit synced();
}
