#include "mainwindow.h"
#include "canvasmodel.h"
#include "canvasview.h"
#include "syncmanager.h"

#include <QToolBar>
#include <QPushButton>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QSize>
#include <QSizePolicy>

MainWindow::MainWindow(const QString &userName, QWidget *parent)
    : QMainWindow(parent),
    m_userName(userName),
      m_model(new CanvasModel(this)),
    m_view(new CanvasView(m_model, m_userName, this)),
      m_sync(new SyncManager(m_model, this))
{
    resize(1024, 768);
    setWindowTitle("Ejercicio 03 - Lienzo colaborativo en tiempo real");

    // Aplicar estilo general sencillo y moderno
    qApp->setFont(QFont("Segoe UI", 10));
    setStyleSheet(
        "QMainWindow { background-color: #f5f7fa; }"
        "QToolBar { background: transparent; spacing: 8px; padding: 6px; }"
        "QToolButton { background-color: #0078D7; color: white; border-radius: 6px; padding: 6px 10px; }"
        "QToolButton:hover { background-color: #005A9E; }"
        "QLabel { color: #333; }"
        "QStatusBar { background: #f0f0f0; }");

    QToolBar *toolbar = addToolBar("Toolbar");
    toolbar->setMovable(false);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolbar->setIconSize(QSize(20, 20));

    QAction *saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Guardar", this);
    saveAction->setToolTip("Guardar en servidor");
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveClicked);
    toolbar->addAction(saveAction);

    QLabel *userLabel = new QLabel(QString("Usuario: %1").arg(m_userName), this);
    userLabel->setContentsMargins(8, 0, 8, 0);
    toolbar->addWidget(userLabel);

    // Spacer para empujar la info a la derecha
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    QLabel *infoLabel = new QLabel("1-9: colores • Rueda: grosor • Izq: dibujar • Der: goma", this);
    infoLabel->setContentsMargins(8, 0, 8, 0);
    toolbar->addWidget(infoLabel);

    setCentralWidget(m_view);

    m_sync->setServerUrl(QUrl("http://194.163.180.102:5000"));
    connect(m_sync, &SyncManager::syncError, this, &MainWindow::onSyncError);

    m_sync->loadFromServer();
    m_sync->startAutoSync(2000);
}

MainWindow::~MainWindow() {}

void MainWindow::onSaveClicked() {
    m_sync->saveToServer();
    statusBar()->showMessage("Guardando en servidor...", 2000);
}

void MainWindow::onSyncError(const QString &message) {
    statusBar()->showMessage("Sync error: " + message, 5000);
}
