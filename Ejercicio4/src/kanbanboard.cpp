#include "kanbanboard.h"
#include "columnwidget.h"
#include "apiclient.h"
#include "carddialog.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

// ── Constructor ───────────────────────────────────────────────────────────────

KanbanBoard::KanbanBoard(ApiClient *api, QWidget *parent)
    : QWidget(parent)
    , m_api(api)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(12, 12, 12, 12);
    m_layout->setSpacing(12);
    m_layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

// ── Public ────────────────────────────────────────────────────────────────────

void KanbanBoard::refreshBoard(const QList<Column> &columns)
{
    m_columns = columns;
    clearColumns();
    for (const Column &col : columns)
        appendColumnWidget(col);
}

// ── Private – layout helpers ──────────────────────────────────────────────────

void KanbanBoard::clearColumns()
{
    for (ColumnWidget *cw : m_colWidgets) {
        m_layout->removeWidget(cw);
        cw->deleteLater();
    }
    m_colWidgets.clear();
}

void KanbanBoard::appendColumnWidget(const Column &col)
{
    auto *cw = new ColumnWidget(col, this);
    connectColumnWidget(cw);
    m_colWidgets.append(cw);
    m_layout->addWidget(cw);
}

void KanbanBoard::connectColumnWidget(ColumnWidget *cw)
{
    connect(cw, &ColumnWidget::addCardRequested,
            this, &KanbanBoard::onAddCardRequested);

    connect(cw, &ColumnWidget::editColumnRequested,
            this, &KanbanBoard::onEditColumnRequested);

    connect(cw, &ColumnWidget::deleteColumnRequested,
            this, &KanbanBoard::onDeleteColumnRequested);

    connect(cw, &ColumnWidget::editCardRequested,
            this, &KanbanBoard::onEditCardRequested);

    connect(cw, &ColumnWidget::deleteCardRequested,
            this, &KanbanBoard::onDeleteCardRequested);

    connect(cw, &ColumnWidget::moveCardLeftRequested,
            this, &KanbanBoard::onMoveCardLeft);

    connect(cw, &ColumnWidget::moveCardRightRequested,
            this, &KanbanBoard::onMoveCardRight);
}

// ── Private – search helpers ──────────────────────────────────────────────────

int KanbanBoard::columnIndexById(int id) const
{
    for (int i = 0; i < m_columns.size(); ++i)
        if (m_columns[i].id == id) return i;
    return -1;
}

Card KanbanBoard::findCard(int cardId) const
{
    for (const Column &col : m_columns)
        for (const Card &card : col.cards)
            if (card.id == cardId) return card;
    return Card{};
}

// ── Slots – Column actions ────────────────────────────────────────────────────

void KanbanBoard::onAddCardRequested(int columnId)
{
    CardDialog dlg(this);
    dlg.setWindowTitle("Nueva tarjeta");
    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.title().isEmpty()) {
        QMessageBox::warning(this, "Error", "El título no puede estar vacío.");
        return;
    }
    m_api->createCard(columnId, dlg.title(), dlg.description());
}

void KanbanBoard::onEditColumnRequested(int columnId, const QString &currentName)
{
    bool ok = false;
    const QString newName = QInputDialog::getText(
        this, "Editar columna", "Nuevo nombre:", QLineEdit::Normal, currentName, &ok);
    if (!ok || newName.trimmed().isEmpty()) return;
    m_api->updateColumn(columnId, newName.trimmed());
}

void KanbanBoard::onDeleteColumnRequested(int columnId)
{
    const int idx = columnIndexById(columnId);
    const QString name = (idx >= 0) ? m_columns[idx].name : QString::number(columnId);

    const auto answer = QMessageBox::question(
        this, "Eliminar columna",
        QString("¿Eliminar la columna \"%1\" y todas sus tarjetas?").arg(name));
    if (answer != QMessageBox::Yes) return;

    m_api->deleteColumn(columnId);
}

// ── Slots – Card actions ──────────────────────────────────────────────────────

void KanbanBoard::onEditCardRequested(int cardId)
{
    const Card card = findCard(cardId);
    if (card.id == 0) return;

    CardDialog dlg(this);
    dlg.setWindowTitle("Editar tarjeta");
    dlg.setTitle(card.title);
    dlg.setDescription(card.description);

    if (dlg.exec() != QDialog::Accepted) return;
    if (dlg.title().isEmpty()) {
        QMessageBox::warning(this, "Error", "El título no puede estar vacío.");
        return;
    }
    m_api->updateCard(cardId, dlg.title(), dlg.description());
}

void KanbanBoard::onDeleteCardRequested(int cardId)
{
    const Card card = findCard(cardId);
    const QString title = (card.id != 0) ? card.title : QString::number(cardId);

    const auto answer = QMessageBox::question(
        this, "Eliminar tarjeta",
        QString("¿Eliminar la tarjeta \"%1\"?").arg(title));
    if (answer != QMessageBox::Yes) return;

    m_api->deleteCard(cardId);
}

void KanbanBoard::onMoveCardLeft(int cardId, int fromColumnId)
{
    const int idx = columnIndexById(fromColumnId);
    if (idx <= 0) return;   // already in the first column
    m_api->moveCard(cardId, m_columns[idx - 1].id);
}

void KanbanBoard::onMoveCardRight(int cardId, int fromColumnId)
{
    const int idx = columnIndexById(fromColumnId);
    if (idx < 0 || idx >= m_columns.size() - 1) return;   // already in the last column
    m_api->moveCard(cardId, m_columns[idx + 1].id);
}
