#pragma once

#include <QDialog>

class QLineEdit;

// Asks the user for server URL, username and password before opening the board.
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString serverUrl() const;
    QString username()  const;
    QString password()  const;

private:
    QLineEdit *m_urlEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;

    void buildUi();
    void loadSettings();
    void saveSettings();
};
