#include "login.h"
#include "ui_login.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>

Login::Login(AdminDB *adminDB, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
    , adminDB(adminDB)
{
    ui->setupUi(this);

    connect(ui->pbIngresar, &QPushButton::clicked, this, &Login::slot_ingresar);
    connect(ui->pbRegistrar, &QPushButton::clicked, this, &Login::slot_registrar);
    connect(ui->pbCancelar, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->leClave, &QLineEdit::returnPressed, this, &Login::slot_ingresar);
}

Login::~Login()
{
    delete ui;
}

void Login::slot_ingresar()
{
    QString usuario = ui->leUsuario->text().trimmed();
    QString clave   = ui->leClave->text();

    if (usuario.isEmpty() || clave.isEmpty()) {
        ui->lbEstado->setText("Ingrese usuario y clave.");
        return;
    }

    QString nombre, apellido;
    if (adminDB->validarUsuario(usuario, clave, nombre, apellido)) {
        adminDB->registrarLog("Acceso exitoso - usuario: " + usuario);
        QMessageBox::information(this, "Bienvenido",
                                 "Bienvenido, " + nombre + " " + apellido + "!");
        accept();
    } else {
        adminDB->registrarLog("Intento fallido - usuario: " + usuario);
        ui->lbEstado->setText("Usuario o clave incorrectos.");
        ui->leClave->clear();
        ui->leClave->setFocus();
    }
}

void Login::slot_registrar()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Registrar usuario");
    dlg.setModal(true);

    QLineEdit *leNuevoUsuario = new QLineEdit(&dlg);
    QLineEdit *leNuevaClave = new QLineEdit(&dlg);
    QLineEdit *leNombre = new QLineEdit(&dlg);
    QLineEdit *leApellido = new QLineEdit(&dlg);
    QLineEdit *leMail = new QLineEdit(&dlg);
    leNuevaClave->setEchoMode(QLineEdit::Password);

    QFormLayout *form = new QFormLayout;
    form->addRow("Usuario:", leNuevoUsuario);
    form->addRow("Clave:", leNuevaClave);
    form->addRow("Nombre:", leNombre);
    form->addRow("Apellido:", leApellido);
    form->addRow("Mail:", leMail);

    QLabel *lbMsg = new QLabel(&dlg);
    lbMsg->setStyleSheet("color: red;");

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        &dlg
    );

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addLayout(form);
    layout->addWidget(lbMsg);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, [&]() {
        QString error;
        const bool ok = adminDB->crearUsuario(
            leNuevoUsuario->text(),
            leNuevaClave->text(),
            leNombre->text(),
            leApellido->text(),
            leMail->text(),
            error
        );

        if (!ok) {
            lbMsg->setText(error);
            return;
        }

        adminDB->registrarLog("Registro exitoso - usuario: " + leNuevoUsuario->text().trimmed());
        QMessageBox::information(this, "Registro", "Usuario creado correctamente.");
        ui->leUsuario->setText(leNuevoUsuario->text().trimmed());
        ui->leClave->clear();
        dlg.accept();
    });

    dlg.exec();
}
