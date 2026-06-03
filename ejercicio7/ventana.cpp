#include "ventana.h"

#include "logger.h"

#include <QFile>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QNetworkReply>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

Ventana::Ventana(const QString &backgroundPath,
                 const QString &profilePhotoPath,
                 const QString &profilePhotoUrl,
                 QWidget *parent)
    : Pantalla(parent)
    , m_backgroundPath(backgroundPath)
    , m_profilePhotoPath(profilePhotoPath)
    , m_profilePhotoUrl(profilePhotoUrl)
{
    configurarPantalla();
}

void Ventana::configurarPantalla()
{
    setWindowTitle("Ventana Principal");
    setMinimumSize(1024, 700);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(32, 32, 32, 32);

    auto *topRow = new QHBoxLayout();
    topRow->addStretch();

    m_card = new QWidget(this);
    m_card->setStyleSheet(
        "background-color: rgba(255, 255, 255, 220);"
        "border: 1px solid rgba(30, 66, 94, 90);"
        "border-radius: 16px;");
    m_card->setFixedWidth(420);

    auto *cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(10);

    auto *title = new QLabel("ejercicio 7 login con clima y fondo de pantalla", m_card);
    title->setStyleSheet("font-size: 30px; font-weight: 700; color: #14324A;");

    auto *role = new QLabel("Software Developer | C++ | Qt", m_card);
    role->setStyleSheet("font-size: 16px; color: #1D5E82; font-weight: 600;");

    m_photoLabel = new QLabel(m_card);
    m_photoLabel->setFixedSize(128, 128);
    m_photoLabel->setStyleSheet("background: #D8E6EF; border-radius: 64px;");
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setText("AL");

    auto *summary = new QLabel(
        "Perfil orientado a desarrollo de aplicaciones robustas con enfoque en calidad, "
        "arquitectura limpia y experiencia de usuario. Experiencia en C++/Qt, integracion "
        "de APIs y solucion de problemas en entornos reales.",
        m_card);
    summary->setWordWrap(true);
    summary->setStyleSheet("font-size: 14px; color: #2D3D4A; line-height: 1.4;");

    auto *experience = new QLabel("Experiencia destacada\n"
                                  "- Desarrollo de interfaces Qt responsivas\n"
                                  "- Integracion de servicios REST y manejo offline\n"
                                  "- Registro de eventos, validaciones y testing funcional",
                                  m_card);
    experience->setWordWrap(true);
    experience->setStyleSheet("font-size: 13px; color: #334B5E;");

    auto *exitHint = new QLabel("Presiona ESC para cerrar esta pantalla.", m_card);
    exitHint->setStyleSheet("font-size: 12px; color: #35556B; font-style: italic;");

    cardLayout->addWidget(title);
    cardLayout->addWidget(role);
    cardLayout->addWidget(m_photoLabel, 0, Qt::AlignHCenter);
    cardLayout->addWidget(summary);
    cardLayout->addWidget(experience);
    cardLayout->addWidget(exitHint);

    topRow->addWidget(m_card);
    root->addLayout(topRow);
    root->addStretch();

    loadBackground();

    if (!m_profilePhotoPath.trimmed().isEmpty()) {
        QPixmap localPhoto;
        if (localPhoto.load(m_profilePhotoPath)) {
            setPhotoPixmap(localPhoto);
            Logger::log("Foto de perfil cargada desde archivo local: " + m_profilePhotoPath);
        } else {
            Logger::log("No se pudo cargar foto local: " + m_profilePhotoPath);
        }
    }

    if (!m_profilePhotoUrl.trimmed().isEmpty()) {
        if (m_photoLabel->pixmap().isNull()) {
            QNetworkReply *reply = m_network.get(QNetworkRequest(QUrl(m_profilePhotoUrl)));
            connect(reply, &QNetworkReply::finished, this, &Ventana::onPhotoDownloaded);
        }
    }

    Logger::log("Ventana principal configurada.");
}

void Ventana::loadBackground()
{
    QFile file(m_backgroundPath);
    if (!file.exists()) {
        Logger::log("No se encontro fondo en cache: " + m_backgroundPath);
        return;
    }

    QPixmap loaded;
    if (!loaded.load(m_backgroundPath)) {
        Logger::log("No se pudo cargar imagen de fondo: " + m_backgroundPath);
        return;
    }

    m_background = loaded;
    Logger::log("Imagen de fondo cargada correctamente.");
}

void Ventana::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (m_background.isNull()) {
        painter.fillRect(rect(), QColor("#1D4E6E"));
        return;
    }

    QPixmap scaled = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    const int x = (width() - scaled.width()) / 2;
    const int y = (height() - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    painter.fillRect(rect(), QColor(15, 30, 45, 60));
}

void Ventana::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        Logger::log("Ventana principal cerrada por tecla ESC.");
        close();
        return;
    }

    Pantalla::keyPressEvent(event);
}

void Ventana::onPhotoDownloaded()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        return;
    }

    QByteArray bytes = reply->readAll();
    reply->deleteLater();

    if (bytes.isEmpty()) {
        Logger::log("La foto de perfil no trajo contenido.");
        return;
    }

    QPixmap photo;
    if (!photo.loadFromData(bytes)) {
        Logger::log("No se pudo parsear la foto de perfil descargada.");
        return;
    }

    setPhotoPixmap(photo);
    Logger::log("Foto de perfil descargada y mostrada.");
}

void Ventana::setPhotoPixmap(const QPixmap &source)
{
    QPixmap scaled = source.scaled(m_photoLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap circular(m_photoLabel->size());
    circular.fill(Qt::transparent);

    QPainter painter(&circular);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addEllipse(circular.rect());
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, scaled);

    m_photoLabel->setPixmap(circular);
}
