#include "canvasmodel.h"
#include <QPainter>
#include <QPainterPath>
#include <QHash>
#include <QStringList>
#include <QtMath>

CanvasModel::CanvasModel(QObject *parent) : QObject(parent) {}

QVector<Stroke> CanvasModel::strokes() const {
    return m_strokes;
}

void CanvasModel::clear() {
    m_strokes.clear();
    emit modelChanged();
    emit localModelChanged();
}

Stroke &CanvasModel::startStroke(bool eraser, const QColor &baseColor, qreal width, const QString &userId) {
    Stroke s;
    s.id = QUuid::createUuid();
    s.eraser = eraser;
    s.width = width;
    s.color = baseColor;
    s.userId = userId;
    m_strokes.append(s);
    emit modelChanged();
    emit localModelChanged();
    return m_strokes.last();
}

void CanvasModel::addPointToStroke(Stroke &stroke, const QPointF &pt) {
    StrokePoint p;
    p.pos = pt;
    p.width = stroke.width;

    if (stroke.eraser) {
        p.color = QColor(Qt::white);
    } else {
        // Mantener color del trazo seleccionado (1-9) de forma consistente
        p.color = stroke.color;
    }

    stroke.points.append(p);
    emit modelChanged();
    emit localModelChanged();
}

void CanvasModel::finishStroke(const Stroke &stroke) {
    Q_UNUSED(stroke)
    emit modelChanged();
    emit localModelChanged();
}

bool CanvasModel::eraseAt(const QPointF &pt, qreal radius, const QString &userId) {
    bool changed = false;

    for (int strokeIndex = m_strokes.size() - 1; strokeIndex >= 0; --strokeIndex) {
        const Stroke stroke = m_strokes.at(strokeIndex);
        if (stroke.eraser || stroke.userId != userId || stroke.points.isEmpty()) {
            continue;
        }

        QVector<QVector<StrokePoint>> fragments;
        QVector<StrokePoint> currentFragment;
        bool strokeChanged = false;

        for (const StrokePoint &point : stroke.points) {
            const qreal dx = point.pos.x() - pt.x();
            const qreal dy = point.pos.y() - pt.y();
            const qreal distance = qSqrt((dx * dx) + (dy * dy));

            if (distance <= radius + (point.width / 2.0)) {
                strokeChanged = true;
                if (!currentFragment.isEmpty()) {
                    fragments.append(currentFragment);
                    currentFragment.clear();
                }
                continue;
            }

            currentFragment.append(point);
        }

        if (!currentFragment.isEmpty()) {
            fragments.append(currentFragment);
        }

        if (!strokeChanged) {
            continue;
        }

        m_strokes.removeAt(strokeIndex);
        changed = true;

        for (const QVector<StrokePoint> &fragment : fragments) {
            if (fragment.isEmpty()) {
                continue;
            }

            Stroke newStroke;
            newStroke.id = QUuid::createUuid();
            newStroke.eraser = false;
            newStroke.width = stroke.width;
            newStroke.color = stroke.color;
            newStroke.userId = stroke.userId;
            newStroke.points = fragment;
            m_strokes.insert(strokeIndex, newStroke);
        }
    }

    if (changed) {
        emit modelChanged();
        emit localModelChanged();
    }

    return changed;
}

void CanvasModel::mergeFrom(const CanvasModel &other) {
    QSet<QUuid> existing;
    for (const Stroke &s : m_strokes) {
        existing.insert(s.id);
    }
    bool changed = false;
    for (const Stroke &s : other.strokes()) {
        if (!existing.contains(s.id)) {
            m_strokes.append(s);
            changed = true;
        }
    }
    if (changed) emit modelChanged();
}

QJsonObject CanvasModel::toJson() const {
    QJsonObject root;
    QJsonArray arr;
    for (const Stroke &s : m_strokes) {
        QJsonObject so;
        so["id"] = s.id.toString();
        so["eraser"] = s.eraser;
        so["userId"] = s.userId;
        QJsonArray pa;
        for (const StrokePoint &p : s.points) {
            QJsonObject po;
            po["x"] = p.pos.x();
            po["y"] = p.pos.y();
            po["width"] = p.width;
            po["color"] = p.color.name();
            pa.append(po);
        }
        so["points"] = pa;
        arr.append(so);
    }
    root["strokes"] = arr;
    return root;
}

bool CanvasModel::fromJson(const QJsonObject &obj) {
    if (!obj.contains("strokes")) return false;
    QJsonArray arr = obj["strokes"].toArray();
    QVector<Stroke> newStrokes;
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject so = v.toObject();
        Stroke s;
        s.id = QUuid(so["id"].toString());
        s.eraser = so["eraser"].toBool();
        s.userId = so["userId"].toString();
        QJsonArray pa = so["points"].toArray();
        for (const QJsonValue &pv : pa) {
            if (!pv.isObject()) continue;
            QJsonObject po = pv.toObject();
            StrokePoint p;
            p.pos = QPointF(po["x"].toDouble(), po["y"].toDouble());
            p.width = po["width"].toDouble();
            p.color = QColor(po["color"].toString());
            s.points.append(p);
        }
        if (!s.points.isEmpty()) {
            s.width = s.points.first().width;
            s.color = s.points.first().color;
        }
        newStrokes.append(s);
    }
    m_strokes = newStrokes;
    emit modelChanged();
    return true;
}

QImage CanvasModel::renderImage(const QSize &size, const QColor &bg) const {
    QImage img(size, QImage::Format_ARGB32_Premultiplied);
    img.fill(bg);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    QHash<QString, QImage> userLayers;
    QStringList userOrder;

    for (const Stroke &stroke : m_strokes) {
        const QString layerKey = stroke.userId.isEmpty() ? QStringLiteral("__anonymous__") : stroke.userId;
        if (!userLayers.contains(layerKey)) {
            QImage layer(size, QImage::Format_ARGB32_Premultiplied);
            layer.fill(Qt::transparent);
            userLayers.insert(layerKey, layer);
            userOrder.append(layerKey);
        }
    }

    for (const Stroke &s : m_strokes) {
        if (s.points.isEmpty()) continue;

        const QString layerKey = s.userId.isEmpty() ? QStringLiteral("__anonymous__") : s.userId;
        QPainter layerPainter(&userLayers[layerKey]);
        layerPainter.setRenderHint(QPainter::Antialiasing);

        if (s.eraser) {
            layerPainter.setCompositionMode(QPainter::CompositionMode_Clear);
        } else {
            layerPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }

        if (s.points.size() == 1) {
            QPen pen;
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setWidthF(s.points.first().width);
            pen.setColor(s.points.first().color);
            layerPainter.setPen(pen);
            layerPainter.drawPoint(s.points.first().pos);
            continue;
        }

        for (int i = 1; i < s.points.size(); ++i) {
            const StrokePoint &p0 = s.points.at(i - 1);
            const StrokePoint &p1 = s.points.at(i);

            QPen pen;
            pen.setCapStyle(Qt::RoundCap);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setWidthF((p0.width + p1.width) / 2.0);
            pen.setColor(p1.color);
            layerPainter.setPen(pen);
            layerPainter.drawLine(p0.pos, p1.pos);
        }
    }

    for (const QString &layerKey : userOrder) {
        painter.drawImage(QPoint(0, 0), userLayers.value(layerKey));
    }

    return img;
}
