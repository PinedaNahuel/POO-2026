QT       += core gui widgets network

CONFIG += c++17

TARGET = ejercicio 3 lienzo colaborativo en tiempo real
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    canvasmodel.cpp \
    canvasview.cpp \
    syncmanager.cpp

HEADERS += \
    mainwindow.h \
    canvasmodel.h \
    canvasview.h \
    syncmanager.h

RESOURCES += \
    
