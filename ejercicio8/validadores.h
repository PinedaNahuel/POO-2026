#ifndef VALIDADORES_H
#define VALIDADORES_H

#include <QString>

class ValidadorSintaxis {
public:
    virtual ~ValidadorSintaxis() = default;
    virtual QString nombreLenguaje() const = 0;
    virtual bool validarLinea(const QString &linea, QString &diagnostico) const = 0;

protected:
    bool balanceDelimitadores(const QString &linea) const;
};

class ValidadorCpp : public ValidadorSintaxis {
public:
    QString nombreLenguaje() const override;
    bool validarLinea(const QString &linea, QString &diagnostico) const override;
};

class ValidadorPython : public ValidadorSintaxis {
public:
    QString nombreLenguaje() const override;
    bool validarLinea(const QString &linea, QString &diagnostico) const override;
};

class ValidadorJava : public ValidadorSintaxis {
public:
    QString nombreLenguaje() const override;
    bool validarLinea(const QString &linea, QString &diagnostico) const override;
};

#endif // VALIDADORES_H
