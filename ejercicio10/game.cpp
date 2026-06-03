#include "game.h"
#include "trex.h"
#include "pajaro.h"
#include <QPainter>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QDebug>
#include <QLabel>
#include <QSvgRenderer>
#include <QPixmap>

GameWindow::GameWindow(QWidget* parent)
    : QWidget(parent)
    , m_trex(new TRex(this))
    , m_mainTimer(new QTimer(this))
    , m_spawnTimer(new QTimer(this))
    , m_cactusSpeed(6)
    , m_gameTimeMs(0)
    , m_gameOver(false)
    , m_speedFactorPercent(100)
{
    setFixedSize(800, 240);
    // QRandomGenerator::global() is already seeded by Qt; do not reseed global generator.
    // position trex on ground
    int groundY = height() - m_trex->height();
    m_trex->move(60, groundY);

    connect(m_mainTimer, SIGNAL(timeout()), this, SLOT(onMainTimer()));
    m_mainTimer->start(20);

    connect(m_spawnTimer, SIGNAL(timeout()), this, SLOT(spawnPajaro()));
    m_spawnTimer->start(5000);
    // spawn an initial bird so player sees birds early
    spawnPajaro();

    // initial cactuses with spacing
    for (int i = 0; i < 3; ++i) {
        createCactus();
    }
    qDebug() << "GameWindow constructed: initial cactuses=" << m_cactuses.size();
}

void GameWindow::restart()
{
    // clear cactuses and birds
    qDeleteAll(m_cactuses);
    m_cactuses.clear();
    qDeleteAll(m_birds);
    m_birds.clear();
    m_gameOver = false;
    m_gameTimeMs = 0;
    m_cactusSpeed = 4;
    m_speedFactorPercent = 100;
    m_trex->reset();
    for (int i = 0; i < 3; ++i) createCactus(200 + i*150, 300);
    // restart timers
    if (!m_mainTimer->isActive()) m_mainTimer->start(30);
    if (!m_spawnTimer->isActive()) m_spawnTimer->start(5000);
    qDebug() << "GameWindow restart: cactuses=" << m_cactuses.size() << "birds=" << m_birds.size();
    update();
}

void GameWindow::createCactus()
{
    // default spacing
    createCactus(200, 600);
}

void GameWindow::createCactus(int minOffset, int extraRange)
{
    QLabel* c = new QLabel(this);
    int cw = 24 + QRandomGenerator::global()->bounded(36);
    int ch = 40 + QRandomGenerator::global()->bounded(30);
    c->setFixedSize(cw, ch);
    // render cactus SVG into pixmap scaled to size
    // prefer PNG resource or file if provided
    QPixmap pm;
    if (!pm.load(QStringLiteral(":/images/cactus.png"))) pm.load(QStringLiteral("images/cactus.png"));
    if (!pm.isNull()) {
        pm = pm.scaled(cw, ch, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        c->setPixmap(pm);
    } else {
        QSvgRenderer renderer(QStringLiteral(":/images/cactus_pixel.svg"));
        if (renderer.isValid() == false) renderer.load(QStringLiteral(":/images/cactus.svg"));
        QPixmap tmp(cw, ch);
        tmp.fill(Qt::transparent);
        QPainter pr(&tmp);
        renderer.render(&pr, QRectF(0,0,cw,ch));
        c->setPixmap(tmp);
    }
    // place relative to last cactus if exists, otherwise relative to window right edge
    int baseX = width();
    if (!m_cactuses.isEmpty()) baseX = m_cactuses.last()->x() + m_cactuses.last()->width();
    int x = baseX + minOffset + QRandomGenerator::global()->bounded(extraRange);
    // avoid producing extremely large x due to previous values
    if (x > width() * 4) x = width() + minOffset + QRandomGenerator::global()->bounded(extraRange);
    int y = height() - c->height();
    c->move(x, y);
    m_cactuses.append(c);
    c->show();
    c->raise();
    qDebug() << "Created cactus at" << c->x() << "size" << c->size();
}

void GameWindow::updateCactuses()
{
    int speed = m_cactusSpeed * m_speedFactorPercent / 100;
    for (QWidget* c : m_cactuses) {
        c->move(c->x() - speed, c->y());
    }
    // add new cactus occasionally but avoid clustering: only if last cactus is far enough
    bool shouldCreate = false;
    if (m_cactuses.isEmpty()) shouldCreate = true;
    else if (m_cactuses.last()->x() < width() - 300) shouldCreate = true;
    else if (QRandomGenerator::global()->bounded(200) == 0) shouldCreate = true; // rare chance
    if (shouldCreate) createCactus();
    // remove offscreen
    for (int i = m_cactuses.size()-1; i >= 0; --i) {
        if (m_cactuses[i]->x() + m_cactuses[i]->width() < 0) {
            // if game over, don't delete so scene stays frozen
            if (m_gameOver) continue;
            delete m_cactuses.takeAt(i);
        }
    }
}

void GameWindow::onMainTimer()
{
    if (m_gameOver) return;
    m_gameTimeMs += m_mainTimer->interval();
    // increase difficulty smoothly: base 6 + 1 per 10s passed
    m_cactusSpeed = 6 + int(m_gameTimeMs / 10000);

    // update trex physics
    m_trex->onTick();

    updateCactuses();
    checkCollisions();
    update();
}

void GameWindow::spawnPajaro()
{
    if (m_gameOver) return;
    // choose among several height bands: low, medium, high
    const int bands[] = {0, 30, 60, 100};
    int heightOffset = bands[QRandomGenerator::global()->bounded(sizeof(bands)/sizeof(bands[0]))];
    int speed = m_cactusSpeed + QRandomGenerator::global()->bounded(3);
    Pajaro* p = new Pajaro(this, heightOffset, speed);
    connect(p, SIGNAL(wentOffscreen(Pajaro*)), this, SLOT(onPajaroOffscreen(Pajaro*)));
    m_birds.append(p);
    p->show();
    p->raise();
    qDebug() << "Spawned bird at" << p->x() << p->y() << "speed" << speed;
}

void GameWindow::onPajaroOffscreen(Pajaro* p)
{
    m_birds.removeOne(p);
    qDebug() << "Pajaro offscreen removed, remaining birds=" << m_birds.size();
    if (p) {
        if (m_gameOver) {
            // keep the bird visible and stopped when game over
            p->stop();
            p->show();
            m_birds.append(p); // keep it in list
            qDebug() << "Pajaro offscreen deferred deletion due to gameOver";
        } else {
            p->deleteLater();
        }
    }
}

void GameWindow::checkCollisions()
{
    QRect rtrex = m_trex->collisionRect();
    for (QWidget* c : m_cactuses) {
        if (!c->isVisible()) continue;
        // shrink cactus hitbox slightly to avoid transparent edges
        QRect rc = c->geometry();
        int cx = int(rc.width() * 0.18);
        int cy = int(rc.height() * 0.12);
        rc.adjust(cx, cy, -cx, -cy);
        if (rtrex.intersects(rc)) { gameOver(); return; }
    }
    for (Pajaro* b : m_birds) {
        if (!b->isVisible()) continue;
        QRect rb = b->collisionRect();
        if (rtrex.intersects(rb)) { gameOver(); return; }
    }
}

void GameWindow::gameOver()
{
    m_gameOver = true;
    // stop main timer;
    m_mainTimer->stop();
    // stop spawn timer and freeze existing birds so scene doesn't empty
    if (m_spawnTimer->isActive()) m_spawnTimer->stop();
    for (Pajaro* b : m_birds) {
        if (b) b->stop();
    }
    qDebug() << "GameOver: cactuses=" << m_cactuses.size() << "birds=" << m_birds.size();
    for (int i=0;i<m_cactuses.size();++i) {
        QWidget* c = m_cactuses[i];
        qDebug() << " cactus" << i << "pos" << c->pos() << "visible" << c->isVisible();
    }
    for (int i=0;i<m_birds.size();++i) {
        Pajaro* b = m_birds[i];
        qDebug() << " bird" << i << "pos" << b->pos() << "visible" << b->isVisible();
    }
    update();
}

void GameWindow::paintEvent(QPaintEvent* /*ev*/)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    // ground
    p.setBrush(QBrush(Qt::gray));
    p.drawRect(0, height()-10, width(), 10);

    if (m_gameOver) {
        p.setPen(Qt::black);
        p.setFont(QFont("Arial", 36, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER\n(R to restart)");
    }
}

void GameWindow::keyPressEvent(QKeyEvent* ev)
{
    if (m_gameOver) {
        if (ev->key() == Qt::Key_R) {
            // restart flow
            m_mainTimer->start(30);
            restart();
        }
        return;
    }

    if (ev->key() == Qt::Key_Space) {
        m_trex->jump();
    } else if (ev->key() == Qt::Key_Down) {
        m_trex->setCrouch(true);
    } else if (ev->key() == Qt::Key_Right) {
        m_speedFactorPercent = 150;
    } else if (ev->key() == Qt::Key_Left) {
        m_speedFactorPercent = 60;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Down) {
        m_trex->setCrouch(false);
    } else if (ev->key() == Qt::Key_Right || ev->key() == Qt::Key_Left) {
        m_speedFactorPercent = 100;
    }
}
