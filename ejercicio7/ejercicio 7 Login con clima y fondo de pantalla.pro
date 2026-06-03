QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appconfig.cpp \
    climaservicio.cpp \
    logger.cpp \
    main.cpp \
    login.cpp \
    pantalla.cpp \
    ventana.cpp

HEADERS += \
    appconfig.h \
    climaservicio.h \
    datosbase.h \
    logger.h \
    login.h \
    pantalla.h \
    ventana.h

FORMS += \
    login.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
