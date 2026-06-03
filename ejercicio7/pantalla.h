#ifndef PANTALLA_H
#define PANTALLA_H

#include <QWidget>

class Pantalla : public QWidget
{
    Q_OBJECT

public:
    explicit Pantalla(QWidget *parent = nullptr);
    ~Pantalla() override = default;

    virtual void configurarPantalla() = 0;
};

#endif // PANTALLA_H
