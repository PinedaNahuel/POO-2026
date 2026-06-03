#pragma once

#include <QDialog>

class QLineEdit;
class QTextEdit;

// Modal dialog used for both creating and editing a card.
class CardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardDialog(QWidget *parent = nullptr);

    void    setTitle(const QString &title);
    void    setDescription(const QString &description);

    QString title()       const;
    QString description() const;

private:
    QLineEdit *m_titleEdit;
    QTextEdit *m_descEdit;
};
