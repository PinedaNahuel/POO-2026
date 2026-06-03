#pragma once

#include <QFrame>
#include "models.h"

class QLabel;

// Displays a single Kanban card with action buttons.
// Emits signals for all user actions; the parent ColumnWidget
// forwards them up to KanbanBoard.
class CardWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CardWidget(const Card &card, QWidget *parent = nullptr);

    int  cardId()   const { return m_card.id; }
    int  columnId() const { return m_card.columnId; }
    void updateCard(const Card &card);

signals:
    void editRequested    (int cardId);
    void deleteRequested  (int cardId);
    void moveLeftRequested(int cardId);
    void moveRightRequested(int cardId);

private:
    Card    m_card;
    QLabel *m_titleLabel;
    QLabel *m_descLabel;

    void buildUi();
    void applyStyle();
};
