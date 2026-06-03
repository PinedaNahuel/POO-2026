#include "pajaro.h"
#include <QPainter>
#include <QRandomGenerator>

Pajaro::Pajaro(QWidget* parent, int heightOffset, int speed)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_speed(speed)
{
    setFixedSize(40, 30);
    // place at right edge
    int x = parent ? parent->width() : 800;
    // ground is few pixels from bottom (same as GameWindow paint)
    int groundY = parent ? parent->height() - 10 : 200;
    // position so bird bottom sits above ground by heightOffset
    int y = groundY - height() - heightOffset;
    move(x, y);

    // Prefer PNG if provided (resource or file), otherwise fall back to SVG renderer
    QPixmap pm;
    if (!pm.load(QStringLiteral(":/images/pajaro.png"))) pm.load(QStringLiteral("images/pajaro.png"));
    if (!pm.isNull()) {
        m_sprite = pm.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        if (!m_renderer.load(QStringLiteral(":/images/pajaro_pixel.svg")))
            m_renderer.load(QStringLiteral(":/images/pajaro.svg"));
        QPixmap tmp(width(), height());
        tmp.fill(Qt::transparent);
        QPainter pr(&tmp);
        m_renderer.render(&pr, QRectF(0,0,width(),height()));
        m_sprite = tmp;
    }

    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->start(20);
}

void Pajaro::setSpeed(int s)
{
    m_speed = s;
}

void Pajaro::stop()
{
    if (m_timer && m_timer->isActive()) m_timer->stop();
}

void Pajaro::start()
{
    if (m_timer && !m_timer->isActive()) m_timer->start(30);
}

void Pajaro::onTimeout()
{
    move(x() - m_speed, y());
    if (x() + width() < 0) {
        // stop timer and notify parent; let parent delete the widget
        m_timer->stop();
        emit wentOffscreen(this);
        // do not call deleteLater() here to avoid destroying during timer callback
        return;
    }
    update();
}

void Pajaro::paintEvent(QPaintEvent* /*ev*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    if (!m_sprite.isNull()) p.drawPixmap(rect(), m_sprite);
}

QRect Pajaro::collisionRect() const
{
    QRect r = geometry();
    // tighten hitbox substantially to match sprite body
    int ix = int(width() * 0.28);
    int iy = int(height() * 0.24);
    r.adjust(ix, iy, -ix, -iy);
    return r;
}
