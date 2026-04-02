QT += widgets network

CONFIG += c++17

TARGET = Ejercicio02
TEMPLATE = app

SOURCES += \
	dashboardwindow.cpp \
	main.cpp \
	monitorclient.cpp

HEADERS += \
	dashboardwindow.h \
	monitorclient.h

DISTFILES += \
	ENDPOINT_FORMAT.md \
	sample_health_response.json
