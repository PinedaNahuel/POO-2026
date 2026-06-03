#pragma once

#include <QFrame>
#include <QList>
#include "models.h"

class QLabel;
class QVBoxLayout;
class CardWidget;

// Displays one Kanban column (header + cards + "add card" button).
// Forwards all user actions as signals for KanbanBoard to handle.
class ColumnWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ColumnWidget(const Column &column, QWidget *parent = nullptr);

    int     columnId()   const { return m_column.id; }
    QString columnName() const { return m_column.name; }

    void refresh(const Column &column);

signals:
    // Column-level actions
    void addCardRequested     (int columnId);
    void editColumnRequested  (int columnId, const QString &currentName);
    void deleteColumnRequested(int columnId);

    // Card-level actions (forwarded from CardWidgets)
    void editCardRequested     (int cardId);
    void deleteCardRequested   (int cardId);
    void moveCardLeftRequested (int cardId, int columnId);
    void moveCardRightRequested(int cardId, int columnId);

private:
    Column       m_column;
    QLabel      *m_nameLabel;
    QVBoxLayout *m_cardsLayout;
    QList<CardWidget *> m_cardWidgets;

    void buildUi();
    void populateCards(const QList<Card> &cards);
    void connectCard(CardWidget *cw);
};
