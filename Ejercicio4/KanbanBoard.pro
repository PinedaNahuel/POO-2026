QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET   = KanbanBoard
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/apiclient.cpp \
    src/logindialog.cpp \
    src/mainwindow.cpp \
    src/kanbanboard.cpp \
    src/columnwidget.cpp \
    src/cardwidget.cpp \
    src/carddialog.cpp

HEADERS += \
    src/models.h \
    src/apiclient.h \
    src/logindialog.h \
    src/mainwindow.h \
    src/kanbanboard.h \
    src/columnwidget.h \
    src/cardwidget.h \
    src/carddialog.h
