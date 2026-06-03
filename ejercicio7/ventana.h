#ifndef VENTANA_H
#define VENTANA_H

#include "pantalla.h"

#include <QNetworkAccessManager>
#include <QPixmap>

class QKeyEvent;
class QLabel;
class QVBoxLayout;

class Ventana : public Pantalla
{
    Q_OBJECT

public:
    explicit Ventana(const QString &backgroundPath,
                     const QString &profilePhotoPath,
                     const QString &profilePhotoUrl,
                     QWidget *parent = nullptr);

    void configurarPantalla() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onPhotoDownloaded();

private:
    QString m_backgroundPath;
    QString m_profilePhotoPath;
    QString m_profilePhotoUrl;
    QPixmap m_background;

    QWidget *m_card = nullptr;
    QLabel *m_photoLabel = nullptr;

    QNetworkAccessManager m_network;
    void loadBackground();
    void setPhotoPixmap(const QPixmap &source);
};

#endif // VENTANA_H
