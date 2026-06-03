#include "mainwindow.h"
#include <QStatusBar>
#include <QLabel>

MainWindow::MainWindow(AdminDB *adminDB, QWidget *parent)
    : QMainWindow(parent)
    , adminDB(adminDB)
{
    setWindowTitle("Pintura - Ejercicio 09");
    resize(900, 650);

    pintura = new Pintura(adminDB, this);
    setCentralWidget(pintura);

    connect(pintura, &Pintura::estadoCambiado,
            this, [this](const QString &msg){ statusBar()->showMessage(msg); });

    // Indicador visual del color actual: recuadro y etiqueta
    lbColorSwatch = new QLabel(this);
    lbColorSwatch->setFixedSize(16, 16);
    lbColorSwatch->setStyleSheet("border:1px solid #000; background-color: black;");

    lbColorName = new QLabel("Negro", this);
    statusBar()->addPermanentWidget(lbColorSwatch);
    statusBar()->addPermanentWidget(lbColorName);

    connect(pintura, &Pintura::colorCambiado, this, [this](const QColor &c){
        lbColorSwatch->setStyleSheet(
            QString("border:1px solid #000; background-color: rgb(%1,%2,%3);")
                .arg(c.red()).arg(c.green()).arg(c.blue())
        );

        QString nombre;
        if (c == Qt::red) nombre = "Rojo";
        else if (c == Qt::green) nombre = "Verde";
        else if (c == Qt::blue) nombre = "Azul";
        else if (c == Qt::black) nombre = "Negro";
        else nombre = c.name().toUpper();

        lbColorName->setText(nombre + QString(" (%1)").arg(c.name()));
    });

    pintura->cargarTrazosDB();

    statusBar()->showMessage(
        "R=Rojo  G=Verde  B=Azul  Rueda=Grosor  Esc=Borrar  Ctrl+Z=Deshacer"
    );
}
