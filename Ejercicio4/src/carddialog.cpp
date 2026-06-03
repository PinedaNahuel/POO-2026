#include "carddialog.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>

CardDialog::CardDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Tarjeta");
    setMinimumWidth(360);

    auto *root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(16, 16, 16, 16);

    auto *form = new QFormLayout();
    form->setSpacing(8);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("Título de la tarjeta");
    form->addRow("Título:", m_titleEdit);

    m_descEdit = new QTextEdit(this);
    m_descEdit->setPlaceholderText("Descripción (opcional)");
    m_descEdit->setMaximumHeight(120);
    form->addRow("Descripción:", m_descEdit);

    root->addLayout(form);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Guardar");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

void CardDialog::setTitle(const QString &title)
{
    m_titleEdit->setText(title);
}

void CardDialog::setDescription(const QString &description)
{
    m_descEdit->setPlainText(description);
}

QString CardDialog::title() const
{
    return m_titleEdit->text().trimmed();
}

QString CardDialog::description() const
{
    return m_descEdit->toPlainText().trimmed();
}
