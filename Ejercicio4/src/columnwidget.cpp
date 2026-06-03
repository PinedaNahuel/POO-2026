#include "columnwidget.h"
#include "cardwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>

// ── Constructor ───────────────────────────────────────────────────────────────

ColumnWidget::ColumnWidget(const Column &column, QWidget *parent)
    : QFrame(parent)
    , m_column(column)
{
    buildUi();
    populateCards(column.cards);
}

// ── Public ────────────────────────────────────────────────────────────────────

void ColumnWidget::refresh(const Column &column)
{
    m_column = column;
    m_nameLabel->setText(column.name);
    populateCards(column.cards);
}

// ── Private ───────────────────────────────────────────────────────────────────

void ColumnWidget::buildUi()
{
    setFixedWidth(280);
    setObjectName("ColumnWidget");
    setStyleSheet(
        "ColumnWidget {"
        "  background: #2b2b2b;"
        "  border: 1px solid #444444;"
        "  border-radius: 8px;"
        "}"
        "QLabel { color: #ffffff; }"
        "QPushButton { color: #ffffff; }"
    );

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QHBoxLayout();
    header->setSpacing(4);

    m_nameLabel = new QLabel(m_column.name, this);
    QFont f = m_nameLabel->font();
    f.setBold(true);
    f.setPointSize(11);
    m_nameLabel->setFont(f);
    m_nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    header->addWidget(m_nameLabel);

    auto *btnEdit = new QPushButton("✎", this);
    btnEdit->setFixedSize(24, 24);
    btnEdit->setFlat(true);
    btnEdit->setToolTip("Editar columna");
    btnEdit->setCursor(Qt::PointingHandCursor);
    header->addWidget(btnEdit);

    auto *btnDel = new QPushButton("✕", this);
    btnDel->setFixedSize(24, 24);
    btnDel->setFlat(true);
    btnDel->setToolTip("Eliminar columna");
    btnDel->setStyleSheet(
        "QPushButton { "
        "  color: #ff6b6b; background: transparent; border: none; "
        "  font-weight: bold; font-size: 14px; "
        "}"
        "QPushButton:hover { color: #ff8787; }"
    );
    btnDel->setCursor(Qt::PointingHandCursor);
    header->addWidget(btnDel);

    root->addLayout(header);

    // ── Scroll area for cards ─────────────────────────────────────────────────
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *cardsContainer = new QWidget(scroll);
    m_cardsLayout = new QVBoxLayout(cardsContainer);
    m_cardsLayout->setContentsMargins(0, 0, 0, 0);
    m_cardsLayout->setSpacing(6);
    m_cardsLayout->addStretch();

    scroll->setWidget(cardsContainer);
    root->addWidget(scroll);

    // ── Add card button ───────────────────────────────────────────────────────
    auto *btnAdd = new QPushButton("+ Agregar tarjeta", this);
    btnAdd->setCursor(Qt::PointingHandCursor);
    btnAdd->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  border: 1px dashed #555555;"
        "  border-radius: 4px;"
        "  padding: 6px;"
        "  color: #aaaaaa;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: #363636; color: #dddddd; }"
    );
    root->addWidget(btnAdd);

    // ── Signal connections ────────────────────────────────────────────────────
    const int colId = m_column.id;
    const QString &name = m_column.name;
    connect(btnEdit, &QPushButton::clicked, this, [this, colId]() {
        emit editColumnRequested(colId, m_column.name);
    });
    connect(btnDel,  &QPushButton::clicked, this, [this, colId]() {
        emit deleteColumnRequested(colId);
    });
    connect(btnAdd,  &QPushButton::clicked, this, [this, colId]() {
        emit addCardRequested(colId);
    });
}

void ColumnWidget::populateCards(const QList<Card> &cards)
{
    // Remove existing card widgets (but keep the trailing stretch)
    for (CardWidget *cw : m_cardWidgets) {
        m_cardsLayout->removeWidget(cw);
        cw->deleteLater();
    }
    m_cardWidgets.clear();

    // Insert new card widgets before the stretch
    const int stretchIndex = m_cardsLayout->count() - 1;
    for (const Card &card : cards) {
        auto *cw = new CardWidget(card, this);
        connectCard(cw);
        m_cardWidgets.append(cw);
        m_cardsLayout->insertWidget(stretchIndex + m_cardWidgets.size() - 1, cw);
    }
}

void ColumnWidget::connectCard(CardWidget *cw)
{
    connect(cw, &CardWidget::editRequested, this, [this](int id) {
        emit editCardRequested(id);
    });
    connect(cw, &CardWidget::deleteRequested, this, [this](int id) {
        emit deleteCardRequested(id);
    });
    connect(cw, &CardWidget::moveLeftRequested, this, [this](int id) {
        emit moveCardLeftRequested(id, m_column.id);
    });
    connect(cw, &CardWidget::moveRightRequested, this, [this](int id) {
        emit moveCardRightRequested(id, m_column.id);
    });
}
