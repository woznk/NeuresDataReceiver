QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../_QT_Libs/hidapi-0.7.0/windows/hid.c \
    cricketapplication.cpp \
    cricketpressure.cpp \
    main.cpp \
    neuresapplication.cpp

HEADERS += \
    ../_QT_Libs/hidapi-0.7.0/hidapi/hidapi.h \
    cricketapplication.h \
    cricketpressure.h \
    neuresapplication.h

FORMS += \
    cricketapplication.ui \
    cricketpressure.ui \
    neuresapplication.ui

TRANSLATIONS += \
    Neures_Data_Receiver_pl_PL.ts


INCLUDEPATH += ../_QT_Libs/hidapi-0.7.0/hidapi

LIBS += ../_QT_Libs/MinGW/libsetupapi.a

RC_ICONS = logo_3lt_1.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    background.qrc
