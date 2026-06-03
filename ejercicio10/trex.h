#pragma once
#include <QWidget>
#include <QTimer>
#include <QPaintEvent>
#include <QPixmap>
#include <QSvgRenderer>

class TRex : public QWidget
{
    Q_OBJECT
public:
    explicit TRex(QWidget* parent = nullptr);
    void jump();
    void setCrouch(bool crouch);
    void setSpeedFactor(double f);
    void reset();
    bool isCrouch() const { return m_crouch; }
    QRect collisionRect() const;

public slots:
    void onTick();

protected:
    void paintEvent(QPaintEvent* ev) override;

private:
    double m_y; // vertical offset from ground (pixels)
    double m_vy; // vertical velocity
    bool m_crouch;
    double m_speedFactor;
    const double m_jumpImpulse = -9.0;
    const double m_gravity = 0.5;
    int m_width;
    int m_height;
    QPixmap m_sprite;
    QSvgRenderer m_renderer;
};
