QT += core gui widgets

CONFIG += c++17

TEMPLATE = app
TARGET = ejercicio 8 editor multilenguaje

SOURCES += \
	main.cpp \
	appconfig.cpp \
	appcontroller.cpp \
	codeeditor.cpp \
	editorprincipal.cpp \
	eventlogger.cpp \
	loginscreen.cpp \
	modobloqueado.cpp \
	validadores.cpp

HEADERS += \
	appconfig.h \
	appcontroller.h \
	codeeditor.h \
	editorprincipal.h \
	eventlogger.h \
	loginscreen.h \
	modobloqueado.h \
	pantalla.h \
	validadores.h
