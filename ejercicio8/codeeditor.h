#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QMap>
#include <QPlainTextEdit>
#include <QSet>

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    int lineaActual() const;
    QString textoLinea(int numeroLineaBase0) const;
    void setErrores(const QSet<int> &lineasInvalidas, const QMap<int, QString> &mensajes);

signals:
    void editorRecibioFoco();
    void editorPerdioFoco(int lineaActual);
    void lineaAbandonada(int lineaBase0);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void onCursorCambio();

private:
    void aplicarResaltado();

    int m_ultimaLinea = 0;
    QSet<int> m_lineasInvalidas;
    QMap<int, QString> m_mensajes;
};

#endif // CODEEDITOR_H
