#pragma once
#include <QWidget>
#include <QTimer>
#include <QPaintEvent>
#include <QPixmap>
#include <QSvgRenderer>

class Pajaro : public QWidget
{
    Q_OBJECT
public:
    explicit Pajaro(QWidget* parent = nullptr, int heightOffset = 0, int speed = 4);
    void setSpeed(int s);
    void stop();
    void start();
    QRect boundingRect() const { return geometry(); }
    QRect collisionRect() const;

signals:
    void wentOffscreen(Pajaro* p);

public slots:
    void onTimeout();

protected:
    void paintEvent(QPaintEvent* ev) override;

private:
    QTimer* m_timer;
    int m_speed;
    QPixmap m_sprite;
    QSvgRenderer m_renderer;
};
