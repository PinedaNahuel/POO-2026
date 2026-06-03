#include "codeeditor.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <utility>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::onCursorCambio);
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
}

int CodeEditor::lineaActual() const {
    return textCursor().blockNumber();
}

QString CodeEditor::textoLinea(int numeroLineaBase0) const {
    QTextBlock block = document()->findBlockByNumber(numeroLineaBase0);
    if (!block.isValid()) {
        return QString();
    }
    return block.text();
}

void CodeEditor::setErrores(const QSet<int> &lineasInvalidas, const QMap<int, QString> &mensajes) {
    m_lineasInvalidas = lineasInvalidas;
    m_mensajes = mensajes;
    aplicarResaltado();
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::mousePressEvent(QMouseEvent *event) {
    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::focusInEvent(QFocusEvent *event) {
    QPlainTextEdit::focusInEvent(event);
    emit editorRecibioFoco();
}

void CodeEditor::focusOutEvent(QFocusEvent *event) {
    emit editorPerdioFoco(lineaActual());
    QPlainTextEdit::focusOutEvent(event);
}

void CodeEditor::onCursorCambio() {
    const int actual = lineaActual();
    if (actual != m_ultimaLinea) {
        emit lineaAbandonada(m_ultimaLinea);
        m_ultimaLinea = actual;
    }
}

void CodeEditor::aplicarResaltado() {
    QList<QTextEdit::ExtraSelection> selections;

    QTextEdit::ExtraSelection actual;
    actual.format.setBackground(QColor(230, 245, 255));
    actual.format.setProperty(QTextFormat::FullWidthSelection, true);
    actual.cursor = textCursor();
    actual.cursor.clearSelection();
    selections.append(actual);

    for (int linea : std::as_const(m_lineasInvalidas)) {
        QTextBlock block = document()->findBlockByNumber(linea);
        if (!block.isValid()) {
            continue;
        }

        QTextEdit::ExtraSelection errorSel;
        errorSel.cursor = QTextCursor(block);
        errorSel.format.setBackground(QColor(255, 220, 220));
        errorSel.format.setProperty(QTextFormat::FullWidthSelection, true);
        selections.append(errorSel);
    }

    setExtraSelections(selections);
}
