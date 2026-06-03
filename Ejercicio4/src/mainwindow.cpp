#include "mainwindow.h"
#include "apiclient.h"
#include "kanbanboard.h"

#include <QScrollArea>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTime>
#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QCryptographicHash>

// ── Constructor ───────────────────────────────────────────────────────────────

MainWindow::MainWindow(const QString &serverUrl,
                       const QString &username,
                       const QString &password,
                       QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Kanban Board");
    resize(1100, 680);

    m_api = new ApiClient(this);
    m_api->setBaseUrl(serverUrl);
    m_api->setCredentials(username, password);

    buildUi();
    setupConnections();
    startPolling();
}

// ── Private ───────────────────────────────────────────────────────────────────

void MainWindow::buildUi()
{
    // ── Toolbar ───────────────────────────────────────────────────────────────
    auto *toolbar = addToolBar("Acciones");
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(16, 16));

    auto *lblTitle = new QLabel(" Kanban Board  ", this);
    QFont f = lblTitle->font();
    f.setBold(true);
    f.setPointSize(13);
    lblTitle->setFont(f);
    toolbar->addWidget(lblTitle);

    toolbar->addSeparator();

    auto *btnAddCol = new QPushButton("＋ Nueva columna", this);
    btnAddCol->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(btnAddCol);

    connect(btnAddCol, &QPushButton::clicked,
            this, &MainWindow::onAddColumnClicked);

    // ── Status bar ────────────────────────────────────────────────────────────
    m_statusLabel = new QLabel("Conectando…", this);
    statusBar()->addPermanentWidget(m_statusLabel);

    // ── Central scroll area ───────────────────────────────────────────────────
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: #1e1e1e;");

    m_board = new KanbanBoard(m_api, this);
    m_board->setStyleSheet("background: #1e1e1e;");
    scroll->setWidget(m_board);

    setCentralWidget(scroll);
}

void MainWindow::setupConnections()
{
    connect(m_api, &ApiClient::boardReceived,
            this,  &MainWindow::onBoardReceived);

    connect(m_api, &ApiClient::operationCompleted,
            this,  &MainWindow::onOperationCompleted);

    connect(m_api, &ApiClient::errorOccurred,
            this,  &MainWindow::onErrorOccurred);
}

void MainWindow::startPolling()
{
    m_api->getBoard();   // immediate first load

    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(3000);   // 3 seconds
    connect(m_pollTimer, &QTimer::timeout,
            m_api, &ApiClient::getBoard);
    m_pollTimer->start();
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void MainWindow::onBoardReceived(const QList<Column> &columns)
{
    const QString newHash = hashColumns(columns);
    if (newHash == m_lastBoardHash) return;   // nothing changed
    m_lastBoardHash = newHash;

    m_board->refreshBoard(columns);
    m_statusLabel->setText(
        QString("Actualizado: %1")
            .arg(QTime::currentTime().toString("hh:mm:ss")));
}

void MainWindow::onOperationCompleted()
{
    // Invalidate cache so the next poll (or immediate refresh) redraws
    m_lastBoardHash.clear();
    m_api->getBoard();
}

void MainWindow::onErrorOccurred(const QString &message)
{
    m_statusLabel->setText("Error: " + message);
}

void MainWindow::onAddColumnClicked()
{
    bool ok = false;
    const QString name = QInputDialog::getText(
        this, "Nueva columna", "Nombre de la columna:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    m_api->createColumn(name.trimmed());
}

// ── Static helper ─────────────────────────────────────────────────────────────

QString MainWindow::hashColumns(const QList<Column> &cols)
{
    QString raw;
    for (const Column &col : cols) {
        raw += QString("%1|%2|%3;").arg(col.id).arg(col.name).arg(col.position);
        for (const Card &card : col.cards) {
            raw += QString("%1|%2|%3|%4|%5,")
                       .arg(card.id).arg(card.columnId)
                       .arg(card.title).arg(card.description)
                       .arg(card.position);
        }
    }
    return QString::fromLatin1(
        QCryptographicHash::hash(raw.toUtf8(), QCryptographicHash::Md5).toHex());
}
