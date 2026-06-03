#include <QApplication>
#include "logindialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Kanban Board");
    app.setOrganizationName("POO");

    // ── Dark theme stylesheet ────────────────────────────────────────────────
    app.setStyle("Fusion");
    app.setStyleSheet(
        "QMainWindow { background: #1e1e1e; }"
        "QWidget { background: #1e1e1e; color: #ffffff; }"
        "QLabel { color: #ffffff; }"
        "QPushButton { "
        "  background: #2d5aa6; color: #ffffff; border: 1px solid #1a3d7a; "
        "  border-radius: 4px; padding: 5px; font-weight: bold; "
        "}"
        "QPushButton:hover { background: #3a6bc9; }"
        "QPushButton:pressed { background: #1a3d7a; }"
        "QLineEdit, QTextEdit { "
        "  background: #2b2b2b; color: #ffffff; border: 1px solid #444444; "
        "  border-radius: 4px; padding: 5px; "
        "}"
        "QLineEdit:focus, QTextEdit:focus { border: 2px solid #2d5aa6; }"
        "QScrollArea { background: #1e1e1e; }"
        "QScrollBar:vertical { background: #2b2b2b; width: 12px; }"
        "QScrollBar::handle:vertical { background: #444444; border-radius: 6px; }"
        "QScrollBar::handle:vertical:hover { background: #555555; }"
        "QInputDialog { background: #1e1e1e; }"
        "QMessageBox { background: #1e1e1e; }"
        "QMessageBox QLabel { color: #ffffff; }"
        "QMessageBox QPushButton { min-width: 70px; }"
        "QToolBar { background: #2b2b2b; border-bottom: 1px solid #444444; }"
        "QStatusBar { background: #2b2b2b; color: #ffffff; border-top: 1px solid #444444; }"
        "QFrame { background: #1e1e1e; }"
    );

    LoginDialog loginDlg;
    if (loginDlg.exec() != QDialog::Accepted)
        return 0;

    MainWindow win(loginDlg.serverUrl(),
                   loginDlg.username(),
                   loginDlg.password());
    win.show();

    return app.exec();
}
