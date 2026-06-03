#ifndef EDITORPRINCIPAL_H
#define EDITORPRINCIPAL_H

#include "appconfig.h"
#include "eventlogger.h"
#include "pantalla.h"
#include "validadores.h"

#include <QMap>
#include <QSet>
#include <memory>

class CodeEditor;
class QComboBox;
class QLabel;
class QPushButton;

class EditorPrincipal : public Pantalla {
    Q_OBJECT

public:
    explicit EditorPrincipal(const AppConfig &config, EventLogger &logger, QWidget *parent = nullptr);

    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() override;
    void registrarEvento(const QString &descripcion) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void onLenguajeCambiado();
    void onLineaAbandonada(int lineaBase0);
    void onEditorPerdioFoco(int lineaActual);
    void onEditorRecibioFoco();
    void exportarAJpg();
    void limpiarCodigo();

private:
    ValidadorSintaxis *validadorActual() const;
    void validarYMarcarLinea(int lineaBase0);
    void refrescarResaltado();
    void cargarTemplateDelLenguaje();

    const AppConfig &m_config;
    EventLogger &m_logger;

    CodeEditor *m_editor = nullptr;
    QComboBox *m_selectorLenguaje = nullptr;
    QLabel *m_estado = nullptr;
    QLabel *m_diagnostico = nullptr;
    QPushButton *m_btnExportar = nullptr;
    QPushButton *m_btnLimpiar = nullptr;

    std::unique_ptr<ValidadorCpp> m_validadorCpp;
    std::unique_ptr<ValidadorPython> m_validadorPython;
    std::unique_ptr<ValidadorJava> m_validadorJava;

    QSet<int> m_lineasInvalidas;
    QMap<int, QString> m_mensajesError;
    bool m_hayCambios = false;
};

#endif // EDITORPRINCIPAL_H
