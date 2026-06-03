#include "editorprincipal.h"

#include "codeeditor.h"
#include "validadores.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFocusEvent>
#include <QFontInfo>
#include <QHBoxLayout>
#include <QImage>
#include <QImageWriter>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QShortcut>
#include <QTextBlock>
#include <QVBoxLayout>

EditorPrincipal::EditorPrincipal(const AppConfig &config, EventLogger &logger, QWidget *parent)
    : Pantalla(parent), m_config(config), m_logger(logger) {
    m_validadorCpp = std::make_unique<ValidadorCpp>();
    m_validadorPython = std::make_unique<ValidadorPython>();
    m_validadorJava = std::make_unique<ValidadorJava>();

    inicializarUI();
    conectarEventos();
}

void EditorPrincipal::inicializarUI() {
    setWindowTitle("Editor Multilenguaje");
    setFocusPolicy(Qt::StrongFocus);

    auto *root = new QHBoxLayout;

    auto *colEditor = new QVBoxLayout;
    auto *toolbar = new QHBoxLayout;

    auto *lblLenguaje = new QLabel("Lenguaje:");
    m_selectorLenguaje = new QComboBox;
    m_selectorLenguaje->addItems({"C++", "Python", "Java"});

    m_btnExportar = new QPushButton("Exportar JPG (Ctrl+S)");
    m_btnLimpiar = new QPushButton("Limpiar");

    toolbar->addWidget(lblLenguaje);
    toolbar->addWidget(m_selectorLenguaje);
    toolbar->addStretch();
    toolbar->addWidget(m_btnLimpiar);
    toolbar->addWidget(m_btnExportar);

    m_editor = new CodeEditor;
    m_editor->setStyleSheet("QPlainTextEdit { background: #f8fbff; font-family: Consolas, 'Courier New', monospace; font-size: 13px; }");

    m_estado = new QLabel("Estado: listo");
    m_diagnostico = new QLabel("Diagnostico: sin errores");
    m_diagnostico->setStyleSheet("color: #254;");

    colEditor->addLayout(toolbar);
    colEditor->addWidget(m_editor, 1);
    colEditor->addWidget(m_estado);
    colEditor->addWidget(m_diagnostico);

    auto *panelCv = new QVBoxLayout;
    auto *cvTitle = new QLabel("Perfil profesional");
    cvTitle->setStyleSheet("font-size: 18px; font-weight: 700;");

    QPixmap foto(150, 150);
    foto.fill(Qt::transparent);
    {
        QPainter p(&foto);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor(29, 78, 216));
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, 150, 150);
        p.setPen(Qt::white);
        p.setFont(QFont("Segoe UI", 36, QFont::Bold));
        p.drawText(foto.rect(), Qt::AlignCenter, "ur");
    }

    auto *fotoLabel = new QLabel;
    fotoLabel->setPixmap(foto);
    fotoLabel->setAlignment(Qt::AlignCenter);

    auto *descripcion = new QLabel("user Landerreche\nDesarrollador C++/Qt\nEnfocado en arquitectura, UX desktop y buenas practicas.");
    descripcion->setWordWrap(true);

    auto *skills = new QLabel("Habilidades\n- C++ / Qt\n- POO y polimorfismo\n- FastAPI y MySQL\n- Git y trabajo colaborativo");
    skills->setWordWrap(true);

    auto *contacto = new QLabel("Contacto\n- GitHub: @user\n- Email: user@example.com");
    contacto->setWordWrap(true);

    panelCv->addWidget(cvTitle);
    panelCv->addWidget(fotoLabel);
    panelCv->addWidget(descripcion);
    panelCv->addWidget(skills);
    panelCv->addWidget(contacto);
    panelCv->addStretch();

    auto *panelCvWidget = new QWidget;
    panelCvWidget->setLayout(panelCv);
    panelCvWidget->setMinimumWidth(280);
    panelCvWidget->setStyleSheet("background: #eef4ff; border: 1px solid #d7e3ff; border-radius: 8px; padding: 8px;");

    root->addLayout(colEditor, 1);
    root->addWidget(panelCvWidget);

    setLayout(root);
}

void EditorPrincipal::conectarEventos() {
    connect(m_selectorLenguaje, &QComboBox::currentTextChanged, this, &EditorPrincipal::onLenguajeCambiado);
    connect(m_btnExportar, &QPushButton::clicked, this, &EditorPrincipal::exportarAJpg);
    connect(m_btnLimpiar, &QPushButton::clicked, this, &EditorPrincipal::limpiarCodigo);

    connect(m_editor, &CodeEditor::lineaAbandonada, this, &EditorPrincipal::onLineaAbandonada);
    connect(m_editor, &CodeEditor::editorPerdioFoco, this, &EditorPrincipal::onEditorPerdioFoco);
    connect(m_editor, &CodeEditor::editorRecibioFoco, this, &EditorPrincipal::onEditorRecibioFoco);
    connect(m_editor, &QPlainTextEdit::textChanged, this, [this]() {
        m_hayCambios = true;
        m_estado->setText("Estado: cambios sin exportar");
    });

    auto *saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &EditorPrincipal::exportarAJpg);
}

void EditorPrincipal::cargarDatos() {
    int idx = m_selectorLenguaje->findText(m_config.lenguajePorDefecto);
    if (idx < 0) {
        idx = 0;
    }
    m_selectorLenguaje->setCurrentIndex(idx);

    m_editor->setPlainText(
        "// Escribi tu codigo aqui\n"
        "// La validacion de sintaxis se ejecuta al abandonar la linea\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n");

    m_lineasInvalidas.clear();
    m_mensajesError.clear();
    refrescarResaltado();
    m_estado->setText("Estado: listo");
    m_diagnostico->setText("Diagnostico: sin errores");
    m_hayCambios = false;

    registrarEvento("Editor cargado con lenguaje " + m_selectorLenguaje->currentText());
}

bool EditorPrincipal::validarEstado() {
    return validadorActual() != nullptr;
}

void EditorPrincipal::registrarEvento(const QString &descripcion) {
    m_logger.log("[Editor] " + descripcion);
}

void EditorPrincipal::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers().testFlag(Qt::ControlModifier) && event->key() == Qt::Key_S) {
        registrarEvento("Atajo Ctrl+S");
        exportarAJpg();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        registrarEvento("Tecla ESC en editor de codigo");
        close();
        return;
    }
    Pantalla::keyPressEvent(event);
}

void EditorPrincipal::mousePressEvent(QMouseEvent *event) {
    registrarEvento("Mouse press en editor principal en " + QString::number(event->position().x()) + "," + QString::number(event->position().y()));
    Pantalla::mousePressEvent(event);
}

void EditorPrincipal::resizeEvent(QResizeEvent *event) {
    registrarEvento("Resize editor a " + QString::number(event->size().width()) + "x" + QString::number(event->size().height()));
    Pantalla::resizeEvent(event);
}

void EditorPrincipal::closeEvent(QCloseEvent *event) {
    const auto res = QMessageBox::question(
        this,
        "Salir",
        "Desea exportar a JPG antes de salir?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::Yes);

    if (res == QMessageBox::Cancel) {
        event->ignore();
        return;
    }

    if (res == QMessageBox::Yes) {
        exportarAJpg();
    }

    registrarEvento("Cierre de aplicacion desde editor");
    event->accept();
}

void EditorPrincipal::focusInEvent(QFocusEvent *event) {
    registrarEvento("Editor principal recibio foco");
    Pantalla::focusInEvent(event);
}

void EditorPrincipal::focusOutEvent(QFocusEvent *event) {
    registrarEvento("Editor principal perdio foco");
    Pantalla::focusOutEvent(event);
}

void EditorPrincipal::onLenguajeCambiado() {
    m_lineasInvalidas.clear();
    m_mensajesError.clear();
    cargarTemplateDelLenguaje();
    refrescarResaltado();
    m_diagnostico->setText("Diagnostico: validando en " + m_selectorLenguaje->currentText());
    registrarEvento("Cambio de lenguaje a " + m_selectorLenguaje->currentText());
}

void EditorPrincipal::onLineaAbandonada(int lineaBase0) {
    validarYMarcarLinea(lineaBase0);
}

void EditorPrincipal::onEditorPerdioFoco(int lineaActual) {
    validarYMarcarLinea(lineaActual);
    registrarEvento("Focus out en editor de codigo");
}

void EditorPrincipal::onEditorRecibioFoco() {
    registrarEvento("Focus in en editor de codigo");
}

void EditorPrincipal::exportarAJpg() {
    if (m_editor->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Exportacion", "El editor esta vacio. Escriba codigo primero.");
        registrarEvento("Intento de exportar con editor vacio");
        return;
    }

    const QString rutaAbsoluta = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + m_config.rutaExportacion);
    QFileInfo info(rutaAbsoluta);
    QDir().mkpath(info.path());

    const QString texto = m_editor->toPlainText();
    const QStringList lineas = texto.split('\n');

    QFont fuente("Consolas", 11);
    if (!QFontInfo(fuente).fixedPitch()) {
        fuente = QFont("Courier New", 11);
    }

    QFontMetrics fm(fuente);
    int maxAncho = 0;
    for (const QString &l : lineas) {
        maxAncho = qMax(maxAncho, fm.horizontalAdvance(l));
    }

    const int margen = 30;
    const int ancho = qMax(1000, maxAncho + margen * 2);
    const int alto = qMax(600, lineas.size() * fm.lineSpacing() + margen * 2);

    QImage img(ancho, alto, QImage::Format_RGB32);
    img.fill(Qt::white);

    QPainter painter(&img);
    painter.setFont(fuente);
    painter.setPen(QColor(25, 25, 25));

    int y = margen + fm.ascent();
    for (const QString &l : lineas) {
        painter.drawText(margen, y, l);
        y += fm.lineSpacing();
    }

    QImageWriter writer(rutaAbsoluta, "jpg");
    writer.setQuality(95);
    const bool ok = writer.write(img);

    if (ok) {
        m_estado->setText("Estado: exportado a " + rutaAbsoluta);
        m_diagnostico->setText("Diagnostico: JPG guardado exitosamente");
        m_diagnostico->setStyleSheet("color: #0b7a2b;");
        registrarEvento("Exportacion JPG exitosa en " + rutaAbsoluta);
        m_hayCambios = false;
        QMessageBox::information(this, "Exportacion", "Codigo exportado a:\n" + rutaAbsoluta);
    } else {
        m_estado->setText("Estado: error al exportar");
        m_diagnostico->setText("Diagnostico: no se pudo escribir JPG (permisos o ruta invalida)");
        m_diagnostico->setStyleSheet("color: #b00020;");
        registrarEvento("Fallo exportacion JPG: " + writer.errorString());
        QMessageBox::critical(this, "Error de exportacion", "No se pudo guardar el archivo JPG.\nVerifique permisos y ruta.\n\nRuta intentada: " + rutaAbsoluta);
    }
}

void EditorPrincipal::limpiarCodigo() {
    m_editor->clear();
    m_lineasInvalidas.clear();
    m_mensajesError.clear();
    refrescarResaltado();
    m_diagnostico->setText("Diagnostico: editor limpio");
    registrarEvento("Se limpio el editor");
}

ValidadorSintaxis *EditorPrincipal::validadorActual() const {
    const QString lenguaje = m_selectorLenguaje->currentText();
    if (lenguaje == "C++") {
        return m_validadorCpp.get();
    }
    if (lenguaje == "Python") {
        return m_validadorPython.get();
    }
    if (lenguaje == "Java") {
        return m_validadorJava.get();
    }
    return nullptr;
}

void EditorPrincipal::validarYMarcarLinea(int lineaBase0) {
    if (lineaBase0 < 0) {
        return;
    }

    ValidadorSintaxis *validador = validadorActual();
    if (!validador) {
        return;
    }

    const QString linea = m_editor->textoLinea(lineaBase0);
    QString diagnostico;
    const bool ok = validador->validarLinea(linea, diagnostico);

    if (ok) {
        m_lineasInvalidas.remove(lineaBase0);
        m_mensajesError.remove(lineaBase0);
        m_diagnostico->setText("Diagnostico (linea " + QString::number(lineaBase0 + 1) + "): valida");
        m_diagnostico->setStyleSheet("color: #0b7a2b;");
    } else {
        m_lineasInvalidas.insert(lineaBase0);
        m_mensajesError[lineaBase0] = diagnostico;
        m_diagnostico->setText("Diagnostico (linea " + QString::number(lineaBase0 + 1) + "): " + diagnostico);
        m_diagnostico->setStyleSheet("color: #b00020;");
    }

    registrarEvento("Validacion de linea " + QString::number(lineaBase0 + 1) + " en " + validador->nombreLenguaje());
    refrescarResaltado();
}

void EditorPrincipal::refrescarResaltado() {
    m_editor->setErrores(m_lineasInvalidas, m_mensajesError);
}

void EditorPrincipal::cargarTemplateDelLenguaje() {
    const QString lenguaje = m_selectorLenguaje->currentText();
    QString template_;

    if (lenguaje == "C++") {
        template_ = R"(// Escribi tu codigo aqui
// La validacion de sintaxis se ejecuta al abandonar la linea
#include <iostream>

int main() {
    std::cout << "Hola mundo" << std::endl;
    return 0;
}
)";
    } else if (lenguaje == "Python") {
        template_ = R"(# Escribi tu codigo aqui
# La validacion de sintaxis se ejecuta al abandonar la linea

def main():
    print("Hola mundo")

if __name__ == "__main__":
    main()
)";
    } else if (lenguaje == "Java") {
        template_ = R"(// Escribi tu codigo aqui
// La validacion de sintaxis se ejecuta al abandonar la linea

public class Main {
    public static void main(String[] args) {
        System.out.println("Hola mundo");
    }
}
)";
    }

    m_editor->setPlainText(template_);
}
