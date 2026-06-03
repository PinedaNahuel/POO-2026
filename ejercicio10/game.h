#pragma once
#include <QWidget>
#include <QTimer>
#include <QList>

class TRex;
class Pajaro;

class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(QWidget* parent = nullptr);
    void restart();

protected:
    void paintEvent(QPaintEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void keyReleaseEvent(QKeyEvent* ev) override;

private slots:
    void onMainTimer();
    void spawnPajaro();
    void onPajaroOffscreen(Pajaro* p);

private:
    TRex* m_trex;
    QTimer* m_mainTimer;
    QTimer* m_spawnTimer;
    QList<QWidget*> m_cactuses;
    QList<Pajaro*> m_birds;
    int m_cactusSpeed;
    int m_gameTimeMs;
    bool m_gameOver;
    int m_speedFactorPercent; // 100 = normal
    void createCactus();
    void createCactus(int minOffset, int extraRange);
    void updateCactuses();
    void checkCollisions();
    void gameOver();
};
