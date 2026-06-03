#pragma once

#include <QString>
#include <QList>

// ── Card ─────────────────────────────────────────────────────────────────────

struct Card {
    int     id          = 0;
    int     columnId    = 0;
    QString title;
    QString description;
    int     position    = 0;
};

// ── Column ───────────────────────────────────────────────────────────────────

struct Column {
    int         id       = 0;
    QString     name;
    int         position = 0;
    QList<Card> cards;
};
