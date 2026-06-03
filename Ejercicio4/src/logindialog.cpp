#include "logindialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QDialogButtonBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Conectar al servidor Kanban");
    setFixedWidth(380);
    buildUi();
    loadSettings();
}

// ── UI construction ───────────────────────────────────────────────────────────

void LoginDialog::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setSpacing(12);
    root->setContentsMargins(20, 20, 20, 20);

    // Title
    auto *title = new QLabel("Kanban Board", this);
    title->setAlignment(Qt::AlignCenter);
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);
    root->addWidget(title);

    // Form
    auto *form = new QFormLayout();
    form->setSpacing(8);

    m_urlEdit  = new QLineEdit(this);
    m_urlEdit->setPlaceholderText("http://localhost:8000");
    form->addRow("URL del servidor:", m_urlEdit);

    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText("admin");
    form->addRow("Usuario:", m_userEdit);

    m_passEdit = new QLineEdit(this);
    m_passEdit->setEchoMode(QLineEdit::Password);
    form->addRow("Contraseña:", m_passEdit);

    root->addLayout(form);

    // Buttons
    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Conectar");
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        saveSettings();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

// ── Settings ──────────────────────────────────────────────────────────────────

void LoginDialog::loadSettings()
{
    QSettings s("POO", "KanbanBoard");
    m_urlEdit->setText(s.value("serverUrl", "http://localhost:8000").toString());
    m_userEdit->setText(s.value("username", "admin").toString());
    // We do NOT persist the password for security reasons
}

void LoginDialog::saveSettings()
{
    QSettings s("POO", "KanbanBoard");
    s.setValue("serverUrl", m_urlEdit->text().trimmed());
    s.setValue("username",  m_userEdit->text().trimmed());
}

// ── Accessors ─────────────────────────────────────────────────────────────────

QString LoginDialog::serverUrl() const { return m_urlEdit->text().trimmed(); }
QString LoginDialog::username()  const { return m_userEdit->text().trimmed(); }
QString LoginDialog::password()  const { return m_passEdit->text(); }
