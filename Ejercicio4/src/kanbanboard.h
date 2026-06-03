#pragma once

#include <QWidget>
#include <QList>
#include "models.h"

class QHBoxLayout;
class ColumnWidget;
class ApiClient;

// The main board widget.  Contains one ColumnWidget per column and
// delegates all API calls to ApiClient.  The board is rebuilt in full
// every time refreshBoard() is called (driven by polling in MainWindow).
class KanbanBoard : public QWidget
{
    Q_OBJECT

public:
    explicit KanbanBoard(ApiClient *api, QWidget *parent = nullptr);

    // Called by MainWindow after every successful getBoard() response
    void refreshBoard(const QList<Column> &columns);

private slots:
    // ── Column actions ────────────────────────────────────────────────────────
    void onAddCardRequested     (int columnId);
    void onEditColumnRequested  (int columnId, const QString &currentName);
    void onDeleteColumnRequested(int columnId);

    // ── Card actions ──────────────────────────────────────────────────────────
    void onEditCardRequested     (int cardId);
    void onDeleteCardRequested   (int cardId);
    void onMoveCardLeft          (int cardId, int fromColumnId);
    void onMoveCardRight         (int cardId, int fromColumnId);

private:
    ApiClient                *m_api;
    QHBoxLayout              *m_layout;
    QList<Column>             m_columns;
    QList<ColumnWidget *>     m_colWidgets;

    void clearColumns();
    void appendColumnWidget(const Column &col);
    void connectColumnWidget(ColumnWidget *cw);

    // Helpers
    int  columnIndexById(int id) const;
    Card findCard(int cardId)    const;
};
