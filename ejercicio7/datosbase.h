#ifndef DATOSBASE_H
#define DATOSBASE_H

#include <QObject>

class DatosBase : public QObject
{
    Q_OBJECT

public:
    explicit DatosBase(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    ~DatosBase() override = default;

    virtual void actualizar() = 0;
};

#endif // DATOSBASE_H
