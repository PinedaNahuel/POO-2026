#include "trex.h"
#include <QPainter>
#include <QColor>

TRex::TRex(QWidget* parent)
    : QWidget(parent)
    , m_y(0)
    , m_vy(0)
    , m_crouch(false)
    , m_speedFactor(1.0)
    , m_width(50)
    , m_height(50)
{
    setFixedSize(m_width, m_height);
    // Prefer PNG if provided (resource or file), otherwise fall back to SVG renderer
    QPixmap pm;
    if (!pm.load(QStringLiteral(":/images/trex.png"))) {
        // try local file path
        pm.load(QStringLiteral("images/trex.png"));
    }
    if (!pm.isNull()) {
        m_sprite = pm.scaled(m_width, m_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // prefer pixel SVG if present
        if (!m_renderer.load(QStringLiteral(":/images/trex_pixel.svg")))
            m_renderer.load(QStringLiteral(":/images/trex.svg"));
        QPixmap tmp(m_width, m_height);
        tmp.fill(Qt::transparent);
        QPainter pr(&tmp);
        m_renderer.render(&pr, QRectF(0,0,m_width,m_height));
        m_sprite = tmp;
    }
}

void TRex::jump()
{
    if (m_y == 0) { // on ground
        m_vy = m_jumpImpulse;
    }
}

void TRex::setCrouch(bool crouch)
{
    m_crouch = crouch;
    // keep widget geometry constant; only change drawing
    update();
}

void TRex::setSpeedFactor(double f)
{
    m_speedFactor = f;
}

void TRex::reset()
{
    m_y = 0;
    m_vy = 0;
    m_crouch = false;
    setFixedSize(m_width, m_height);
    if (parentWidget()) {
        int groundY = parentWidget()->height() - height();
        move(x(), groundY + (int)m_y);
    }
    update();
}

void TRex::onTick()
{
    // simple physics: update vertical movement if in air
    if (m_y != 0 || m_vy != 0) {
        m_vy += m_gravity;
        m_y += m_vy;
        if (m_y > 0) {
            m_y = 0;
            m_vy = 0;
        }
    }
    // always ensure widget sits on ground + m_y
    if (parentWidget()) {
        int groundY = parentWidget()->height() - height();
        move(x(), groundY + (int)m_y);
    }
}

QRect TRex::collisionRect() const
{
    QRect r = geometry();
    // reduce hitbox to better match sprite: inset horizontally and from top
    int ix = int(width() * 0.15);
    int iyTop = int(height() * 0.12);
    int iyBottom = m_crouch ? int(height() * 0.05) : 0;
    r.adjust(ix, iyTop, -ix, -iyBottom);
    return r;
}

void TRex::paintEvent(QPaintEvent* /*ev*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QBrush(Qt::darkGreen));
    // draw full bounding rounded rect
    QRect body = rect();
    if (m_sprite.isNull()) {
        p.fillRect(rect(), Qt::darkGreen);
        return;
    }
    if (m_crouch) {
        // draw cropped sprite (lower half)
        QRect src(0, m_sprite.height()/2, m_sprite.width(), m_sprite.height()/2);
        QRect dst(0, height()/2, width(), height()/2);
        p.drawPixmap(dst, m_sprite, src);
    } else {
        p.drawPixmap(rect(), m_sprite);
    }
}
