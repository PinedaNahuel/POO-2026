#include "pintura.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Pintura::Pintura(AdminDB *adminDB, QWidget *parent)
    : QWidget(parent)
    , adminDB(adminDB)
    , dibujando(false)
    , colorActual(Qt::black)
    , grosorActual(3)
    , undoDisponible(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(600, 400);
    setAutoFillBackground(true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

// ---------------------------------------------------------------------------
// Carga inicial desde la base de datos
// ---------------------------------------------------------------------------
void Pintura::cargarTrazosDB()
{
    trazos.clear();

    QSqlDatabase db = adminDB->getDB();
    QSqlQuery queryTrazos("SELECT id FROM trazos ORDER BY id", db);

    while (queryTrazos.next()) {
        Trazo t;
        t.dbId = queryTrazos.value(0).toInt();

        QSqlQuery queryPuntos(db);
        queryPuntos.prepare(
            "SELECT x, y, r, g, b, grosor FROM puntos "
            "WHERE trazo_id = :id ORDER BY id"
        );
        queryPuntos.bindValue(":id", t.dbId);
        queryPuntos.exec();

        while (queryPuntos.next()) {
            Punto p;
            p.x      = queryPuntos.value(0).toInt();
            p.y      = queryPuntos.value(1).toInt();
            p.r      = queryPuntos.value(2).toInt();
            p.g      = queryPuntos.value(3).toInt();
            p.b      = queryPuntos.value(4).toInt();
            p.grosor = queryPuntos.value(5).toInt();
            t.puntos.append(p);
        }

        if (!t.puntos.isEmpty())
            trazos.append(t);
    }

    undoDisponible = qMin(trazos.size(), 10);
    update();
    emitirEstado();
}

// ---------------------------------------------------------------------------
// Dibujo
// ---------------------------------------------------------------------------
void Pintura::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);

    auto dibujarTrazo = [&](const Trazo &t) {
        if (t.puntos.isEmpty())
            return;

        if (t.puntos.size() == 1) {
            const Punto &p = t.puntos.first();
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(p.r, p.g, p.b));
            int radio = qMax(1, p.grosor / 2);
            painter.drawEllipse(QPoint(p.x, p.y), radio, radio);
            return;
        }

        for (int i = 1; i < t.puntos.size(); ++i) {
            const Punto &p0 = t.puntos[i - 1];
            const Punto &p1 = t.puntos[i];
            QPen pen(QColor(p1.r, p1.g, p1.b));
            pen.setWidth(p1.grosor);
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            painter.setPen(pen);
            painter.drawLine(p0.x, p0.y, p1.x, p1.y);
        }
    };

    for (const Trazo &t : trazos)
        dibujarTrazo(t);

    dibujarTrazo(trazoCurrent);
}

// ---------------------------------------------------------------------------
// Eventos de mouse
// ---------------------------------------------------------------------------
void Pintura::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dibujando = true;
        trazoCurrent.puntos.clear();
        trazoCurrent.dbId = -1;

        Punto p;
        p.x      = event->pos().x();
        p.y      = event->pos().y();
        p.r      = colorActual.red();
        p.g      = colorActual.green();
        p.b      = colorActual.blue();
        p.grosor = grosorActual;
        trazoCurrent.puntos.append(p);
        update();
    }
}

void Pintura::mouseMoveEvent(QMouseEvent *event)
{
    if (dibujando && (event->buttons() & Qt::LeftButton)) {
        Punto p;
        p.x      = event->pos().x();
        p.y      = event->pos().y();
        p.r      = colorActual.red();
        p.g      = colorActual.green();
        p.b      = colorActual.blue();
        p.grosor = grosorActual;
        trazoCurrent.puntos.append(p);
        update();
    }
}

void Pintura::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && dibujando) {
        dibujando = false;

        if (!trazoCurrent.puntos.isEmpty()) {
            int id = guardarTrazo(trazoCurrent);
            trazoCurrent.dbId = id;
            trazos.append(trazoCurrent);
            trazoCurrent.puntos.clear();

            if (undoDisponible < 10)
                undoDisponible++;
        }
        update();
    }
}

// ---------------------------------------------------------------------------
// Rueda del mouse: ajusta grosor del pincel
// ---------------------------------------------------------------------------
void Pintura::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        grosorActual = qMin(grosorActual + 1, 50);
    else
        grosorActual = qMax(grosorActual - 1, 1);

    event->accept();
    emitirEstado();
}

// ---------------------------------------------------------------------------
// Teclado
// ---------------------------------------------------------------------------
void Pintura::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_R:
        colorActual = Qt::red;
        emitirEstado();
        break;
    case Qt::Key_G:
        colorActual = Qt::green;
        emitirEstado();
        break;
    case Qt::Key_B:
        colorActual = Qt::blue;
        emitirEstado();
        break;
    case Qt::Key_Escape:
        // Borrar lienzo completo
        trazos.clear();
        trazoCurrent.puntos.clear();
        undoDisponible = 0;
        limpiarTrazosDB();
        update();
        emitirEstado();
        break;
    case Qt::Key_Z:
        // Ctrl+Z: deshacer último trazo (máximo 10 veces)
        if ((event->modifiers() & Qt::ControlModifier) &&
            !trazos.isEmpty() && undoDisponible > 0)
        {
            Trazo ultimo = trazos.takeLast();
            if (ultimo.dbId >= 0)
                eliminarTrazo(ultimo.dbId);
            undoDisponible--;
            update();
            emitirEstado();
        }
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

// ---------------------------------------------------------------------------
// Operaciones con la base de datos
// ---------------------------------------------------------------------------
int Pintura::guardarTrazo(const Trazo &trazo)
{
    QSqlDatabase db = adminDB->getDB();

    QSqlQuery qTrazo(db);
    qTrazo.exec("INSERT INTO trazos (id) VALUES (NULL)");
    int trazoid = qTrazo.lastInsertId().toInt();

    QSqlQuery qPunto(db);
    for (const Punto &p : trazo.puntos) {
        qPunto.prepare(
            "INSERT INTO puntos (trazo_id, x, y, r, g, b, grosor) "
            "VALUES (:tid, :x, :y, :r, :g, :b, :gr)"
        );
        qPunto.bindValue(":tid", trazoid);
        qPunto.bindValue(":x",   p.x);
        qPunto.bindValue(":y",   p.y);
        qPunto.bindValue(":r",   p.r);
        qPunto.bindValue(":g",   p.g);
        qPunto.bindValue(":b",   p.b);
        qPunto.bindValue(":gr",  p.grosor);

        if (!qPunto.exec())
            qDebug() << "Error insertando punto:" << qPunto.lastError().text();
    }

    return trazoid;
}

void Pintura::eliminarTrazo(int dbId)
{
    QSqlDatabase db = adminDB->getDB();

    QSqlQuery q(db);
    q.prepare("DELETE FROM puntos WHERE trazo_id = :id");
    q.bindValue(":id", dbId);
    q.exec();

    q.prepare("DELETE FROM trazos WHERE id = :id");
    q.bindValue(":id", dbId);
    q.exec();
}

void Pintura::limpiarTrazosDB()
{
    QSqlDatabase db = adminDB->getDB();
    QSqlQuery q(db);
    q.exec("DELETE FROM puntos");
    q.exec("DELETE FROM trazos");
}

void Pintura::emitirEstado()
{
    QString colorNombre;
    if (colorActual == Qt::red)         colorNombre = "Rojo";
    else if (colorActual == Qt::green)  colorNombre = "Verde";
    else if (colorActual == Qt::blue)   colorNombre = "Azul";
    else                                colorNombre = "Negro";

    emit estadoCambiado(
        QString("Color: %1  |  Grosor: %2  |  Deshacer disponible: %3/10")
            .arg(colorNombre)
            .arg(grosorActual)
            .arg(undoDisponible)
    );
    // También notifica el color actual para que la UI muestre un swatch
    emit colorCambiado(colorActual);
}
