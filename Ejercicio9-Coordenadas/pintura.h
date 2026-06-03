#ifndef PINTURA_H
#define PINTURA_H

#include <QWidget>
#include <QList>
#include <QColor>
#include "admindb.h"

// Representa un punto individual del trazo con sus propiedades visuales
struct Punto {
    int x, y;
    int r, g, b;
    int grosor;
};

// Agrupa los puntos de un trazo continuo y su ID en la base de datos
struct Trazo {
    int         dbId = -1;
    QList<Punto> puntos;
};

class Pintura : public QWidget
{
    Q_OBJECT

public:
    explicit Pintura(AdminDB *adminDB, QWidget *parent = nullptr);

    // Carga los trazos guardados en la base de datos al iniciar
    void cargarTrazosDB();

signals:
    // Notifica cambios de estado (color, grosor) para la barra de estado
    void estadoCambiado(const QString &mensaje);
    // Notifica cuando cambia el color actual (para mostrar un swatch en la UI)
    void colorCambiado(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event)       override;
    void mousePressEvent(QMouseEvent *event)  override;
    void mouseMoveEvent(QMouseEvent *event)   override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event)       override;
    void keyPressEvent(QKeyEvent *event)      override;

private:
    AdminDB      *adminDB;
    QList<Trazo>  trazos;         // Todos los trazos completados
    Trazo         trazoCurrent;   // Trazo que se está dibujando
    bool          dibujando;
    QColor        colorActual;
    int           grosorActual;
    int           undoDisponible; // Máximo 10 deshaceres disponibles

    int  guardarTrazo(const Trazo &trazo);
    void eliminarTrazo(int dbId);
    void limpiarTrazosDB();
    void emitirEstado();
};

#endif // PINTURA_H
