#pragma once

#include <QMainWindow>
#include <QList>
#include "models.h"

class QTimer;
class QLabel;
class QScrollArea;
class ApiClient;
class KanbanBoard;

// Top-level application window.
// Owns the ApiClient and drives the polling timer (every 3 seconds).
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &serverUrl,
                        const QString &username,
                        const QString &password,
                        QWidget *parent = nullptr);

private slots:
    void onBoardReceived     (const QList<Column> &columns);
    void onOperationCompleted();
    void onErrorOccurred     (const QString &message);
    void onAddColumnClicked();

private:
    ApiClient   *m_api;
    KanbanBoard *m_board;
    QTimer      *m_pollTimer;
    QLabel      *m_statusLabel;

    // Used to avoid unnecessary UI rebuilds when data hasn't changed
    QString      m_lastBoardHash;

    void buildUi();
    void setupConnections();
    void startPolling();

    static QString hashColumns(const QList<Column> &cols);
};
