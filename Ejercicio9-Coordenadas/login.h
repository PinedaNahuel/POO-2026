#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "admindb.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(AdminDB *adminDB, QWidget *parent = nullptr);
    ~Login();

private slots:
    void slot_ingresar();
    void slot_registrar();

private:
    Ui::Login *ui;
    AdminDB   *adminDB;
};

#endif // LOGIN_H
