#include "cardwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// ── Constructor ───────────────────────────────────────────────────────────────

CardWidget::CardWidget(const Card &card, QWidget *parent)
    : QFrame(parent)
    , m_card(card)
{
    buildUi();
    applyStyle();
}

// ── Public ────────────────────────────────────────────────────────────────────

void CardWidget::updateCard(const Card &card)
{
    m_card = card;
    m_titleLabel->setText(card.title);

    const QString shortDesc = card.description.left(100) +
                              (card.description.length() > 100 ? "…" : "");
    m_descLabel->setText(shortDesc);
    m_descLabel->setVisible(!shortDesc.isEmpty());
}

// ── Private ───────────────────────────────────────────────────────────────────

void CardWidget::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 6, 8, 6);
    root->setSpacing(4);

    // Title
    m_titleLabel = new QLabel(m_card.title, this);
    m_titleLabel->setWordWrap(true);
    QFont f = m_titleLabel->font();
    f.setBold(true);
    m_titleLabel->setFont(f);
    root->addWidget(m_titleLabel);

    // Description (truncated)
    const QString shortDesc = m_card.description.left(100) +
                              (m_card.description.length() > 100 ? "…" : "");
    m_descLabel = new QLabel(shortDesc, this);
    m_descLabel->setWordWrap(true);
    m_descLabel->setVisible(!shortDesc.isEmpty());
    m_descLabel->setStyleSheet("color: #aaaaaa; font-size: 11px;");
    root->addWidget(m_descLabel);

    // Action buttons row
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(4);

    auto *btnLeft   = new QPushButton("◀", this);
    auto *btnEdit   = new QPushButton("✎", this);
    auto *btnDelete = new QPushButton("✕", this);
    auto *btnRight  = new QPushButton("▶", this);

    for (QPushButton *btn : {btnLeft, btnEdit, btnDelete, btnRight}) {
        btn->setFixedSize(28, 24);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { color: #888888; background: transparent; border: none; }"
            "QPushButton:hover { color: #ffffff; }"
        );
    }
    btnDelete->setStyleSheet(
        "QPushButton { color: #ff6b6b; background: transparent; border: none; font-weight: bold; }"
        "QPushButton:hover { color: #ff8787; }"
    );

    btnRow->addWidget(btnLeft);
    btnRow->addWidget(btnEdit);
    btnRow->addStretch();
    btnRow->addWidget(btnDelete);
    btnRow->addWidget(btnRight);
    root->addLayout(btnRow);

    // Connections
    const int id = m_card.id;
    connect(btnLeft,   &QPushButton::clicked, this, [this, id]() { emit moveLeftRequested(id); });
    connect(btnRight,  &QPushButton::clicked, this, [this, id]() { emit moveRightRequested(id); });
    connect(btnEdit,   &QPushButton::clicked, this, [this, id]() { emit editRequested(id); });
    connect(btnDelete, &QPushButton::clicked, this, [this, id]() { emit deleteRequested(id); });
}

void CardWidget::applyStyle()
{
    setFrameShape(QFrame::StyledPanel);
    setObjectName("CardWidget");
    setStyleSheet(
        "CardWidget {"
        "  background: #363636;"
        "  border: 1px solid #555555;"
        "  border-radius: 6px;"
        "  color: #ffffff;"
        "}"
        "CardWidget:hover {"
        "  border-color: #2d5aa6; background: #3d3d3d;"
        "}"
        "QLabel { color: #ffffff; }"
    );
}
