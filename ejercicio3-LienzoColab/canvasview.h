#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QWidget>
#include <QColor>
#include <QPointF>
#include <QTimer>
#include <QString>
#include "canvasmodel.h"

class CanvasView : public QWidget {
    Q_OBJECT
public:
    explicit CanvasView(CanvasModel *model, const QString &userId, QWidget *parent = nullptr);

    void setBaseColor(const QColor &color);
    void setPenWidth(qreal width);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateModel();

private:
    enum Tool { Pen, Eraser };

    CanvasModel *m_model;
    QString m_userId;
    QColor m_baseColor;
    qreal m_penWidth;
    qreal m_eraserWidth;
    bool m_drawing;
    Tool m_tool;
    Stroke *m_currentStroke;

    QColor colorForKey(int key);
};

#endif // CANVASVIEW_H