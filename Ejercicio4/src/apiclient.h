#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "models.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    void setBaseUrl(const QString &url);
    void setCredentials(const QString &username, const QString &password);

    // ── Board ─────────────────────────────────────────────────────────────────
    void getBoard();

    // ── Columns ───────────────────────────────────────────────────────────────
    void createColumn(const QString &name);
    void updateColumn(int id, const QString &name);
    void deleteColumn(int id);

    // ── Cards ─────────────────────────────────────────────────────────────────
    void createCard(int columnId, const QString &title, const QString &description);
    void updateCard(int id, const QString &title, const QString &description);
    void deleteCard(int id);
    void moveCard(int cardId, int newColumnId);
    void reorderCards(int columnId, const QList<int> &cardIds);

signals:
    void boardReceived(const QList<Column> &columns);
    void operationCompleted();
    void errorOccurred(const QString &message);

private slots:
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *auth);

private:
    QNetworkAccessManager *m_manager;
    QString                m_baseUrl;
    QString                m_username;
    QString                m_password;
    QByteArray             m_authHeader;   // "Basic <base64>"

    QNetworkRequest buildRequest(const QString &path) const;

    // Connect reply -> delete + emit operationCompleted (or errorOccurred)
    void connectWriteReply(QNetworkReply *reply);

    static QList<Column> parseColumns(const QJsonArray &arr);
};
