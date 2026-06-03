#include "apiclient.h"

#include <QNetworkReply>
#include <QAuthenticator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

// ── Constructor ───────────────────────────────────────────────────────────────

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    // Connect to authentication required signal
    connect(m_manager, &QNetworkAccessManager::authenticationRequired,
            this, &ApiClient::onAuthenticationRequired);
}

// ── Configuration ─────────────────────────────────────────────────────────────

void ApiClient::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
    // Ensure no trailing slash
    while (m_baseUrl.endsWith('/'))
        m_baseUrl.chop(1);
}

void ApiClient::setCredentials(const QString &username, const QString &password)
{
    m_username = username;
    m_password = password;
    const QString credentials = username + ':' + password;
    m_authHeader = "Basic " + credentials.toUtf8().toBase64();
}

// ── Slots ──────────────────────────────────────────────────────────────────────

void ApiClient::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *auth)
{
    // Qt is asking for credentials when it receives a 401.
    // We provide them here, but they're also sent in the header preemptively.
    auth->setUser(m_username);
    auth->setPassword(m_password);
}

// ── Private helpers ───────────────────────────────────────────────────────────

QNetworkRequest ApiClient::buildRequest(const QString &path) const
{
    QNetworkRequest req;
    req.setUrl(QUrl(m_baseUrl + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", m_authHeader);
    return req;
}

void ApiClient::connectWriteReply(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }
        const int httpStatus =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (httpStatus >= 400) {
            emit errorOccurred(
                QString("Error del servidor: %1").arg(httpStatus));
            return;
        }
        emit operationCompleted();
    });
}

// ── Static parser ─────────────────────────────────────────────────────────────

QList<Column> ApiClient::parseColumns(const QJsonArray &arr)
{
    QList<Column> columns;
    for (const QJsonValue &colVal : arr) {
        const QJsonObject colObj = colVal.toObject();
        Column col;
        col.id       = colObj["id"].toInt();
        col.name     = colObj["name"].toString();
        col.position = colObj["position"].toInt();

        const QJsonArray cardsArr = colObj["cards"].toArray();
        for (const QJsonValue &cardVal : cardsArr) {
            const QJsonObject cardObj = cardVal.toObject();
            Card card;
            card.id          = cardObj["id"].toInt();
            card.columnId    = cardObj["column_id"].toInt();
            card.title       = cardObj["title"].toString();
            card.description = cardObj["description"].toString();
            card.position    = cardObj["position"].toInt();
            col.cards.append(card);
        }
        columns.append(col);
    }
    return columns;
}

// ── Board ─────────────────────────────────────────────────────────────────────

void ApiClient::getBoard()
{
    QNetworkReply *reply = m_manager->get(buildRequest("/columns/"));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }
        const int httpStatus =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (httpStatus >= 400) {
            if (httpStatus == 401) {
                emit errorOccurred("Credenciales inválidas (usuario/contraseña incorrectos)");
            } else {
                emit errorOccurred(QString("Error del servidor: %1").arg(httpStatus));
            }
            return;
        }
        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit boardReceived(parseColumns(doc.array()));
    });
}

// ── Columns ───────────────────────────────────────────────────────────────────

void ApiClient::createColumn(const QString &name)
{
    QJsonObject body;
    body["name"] = name;
    QNetworkReply *reply =
        m_manager->post(buildRequest("/columns/"), QJsonDocument(body).toJson());
    connectWriteReply(reply);
}

void ApiClient::updateColumn(int id, const QString &name)
{
    QJsonObject body;
    body["name"] = name;
    QNetworkReply *reply =
        m_manager->put(buildRequest(QString("/columns/%1").arg(id)),
                       QJsonDocument(body).toJson());
    connectWriteReply(reply);
}

void ApiClient::deleteColumn(int id)
{
    QNetworkReply *reply =
        m_manager->deleteResource(buildRequest(QString("/columns/%1").arg(id)));
    connectWriteReply(reply);
}

// ── Cards ─────────────────────────────────────────────────────────────────────

void ApiClient::createCard(int columnId, const QString &title, const QString &description)
{
    QJsonObject body;
    body["column_id"]   = columnId;
    body["title"]       = title;
    body["description"] = description;
    QNetworkReply *reply =
        m_manager->post(buildRequest("/cards/"), QJsonDocument(body).toJson());
    connectWriteReply(reply);
}

void ApiClient::updateCard(int id, const QString &title, const QString &description)
{
    QJsonObject body;
    body["title"]       = title;
    body["description"] = description;
    QNetworkReply *reply =
        m_manager->put(buildRequest(QString("/cards/%1").arg(id)),
                       QJsonDocument(body).toJson());
    connectWriteReply(reply);
}

void ApiClient::deleteCard(int id)
{
    QNetworkReply *reply =
        m_manager->deleteResource(buildRequest(QString("/cards/%1").arg(id)));
    connectWriteReply(reply);
}

void ApiClient::moveCard(int cardId, int newColumnId)
{
    QJsonObject body;
    body["new_column_id"] = newColumnId;
    QNetworkReply *reply =
        m_manager->put(buildRequest(QString("/cards/%1/move").arg(cardId)),
                       QJsonDocument(body).toJson());
    connectWriteReply(reply);
}

void ApiClient::reorderCards(int columnId, const QList<int> &cardIds)
{
    QJsonArray arr;
    for (int id : cardIds)
        arr.append(id);
    QJsonObject body;
    body["card_ids"] = arr;
    QNetworkReply *reply =
        m_manager->put(buildRequest(QString("/columns/%1/reorder").arg(columnId)),
                       QJsonDocument(body).toJson());
    connectWriteReply(reply);
}
