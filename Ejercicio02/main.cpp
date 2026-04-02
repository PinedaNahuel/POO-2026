#include <QApplication>

#include "dashboardwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Cloud VPS Monitor");
    app.setOrganizationName("Ejercicio02");

    DashboardWindow window;
    window.show();

    return app.exec();
}