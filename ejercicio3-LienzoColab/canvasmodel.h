#ifndef CANVASMODEL_H
#define CANVASMODEL_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QVector>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

struct StrokePoint {
    QPointF pos;
    qreal width;
    QColor color;
};

struct Stroke {
    QUuid id;
    QVector<StrokePoint> points;
    bool eraser = false;
    qreal width = 1.0;
    QColor color = QColor(192, 19, 76);
    QString userId;
};

class CanvasModel : public QObject {
    Q_OBJECT
public:
    explicit CanvasModel(QObject *parent = nullptr);

    QVector<Stroke> strokes() const;
    void clear();

    Stroke &startStroke(bool eraser, const QColor &baseColor, qreal width, const QString &userId);
    void addPointToStroke(Stroke &stroke, const QPointF &pt);
    void finishStroke(const Stroke &stroke);
    bool eraseAt(const QPointF &pt, qreal radius, const QString &userId);

    void mergeFrom(const CanvasModel &other);

    QJsonObject toJson() const;
    bool fromJson(const QJsonObject &obj);

    QImage renderImage(const QSize &size, const QColor &bg = Qt::white) const;

signals:
    void modelChanged();
    void localModelChanged();

private:
    QVector<Stroke> m_strokes;
};

#endif // CANVASMODEL_H