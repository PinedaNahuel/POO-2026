#include "mainwindow.h"
#include <QApplication>
#include <QInputDialog>
#include <QLineEdit>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    bool accepted = false;
    QString userName;
    while (!accepted) {
        bool ok = false;
        userName = QInputDialog::getText(
            nullptr,
            "Ingresar usuario",
            "Nombre de usuario:",
            QLineEdit::Normal,
            QString(),
            &ok
        ).trimmed();

        if (!ok) {
            return 0;
        }

        if (!userName.isEmpty()) {
            accepted = true;
        }
    }

    MainWindow w(userName);
    w.show();
    return a.exec();
}
