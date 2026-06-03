#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class CanvasModel;
class CanvasView;
class SyncManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const QString &userName, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSaveClicked();
    void onSyncError(const QString &message);

private:
    QString m_userName;
    CanvasModel *m_model;
    CanvasView *m_view;
    SyncManager *m_sync;
};

#endif // MAINWINDOW_H