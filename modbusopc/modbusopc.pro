QT       += core gui
LIBS += -Ldll -lws2_32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =modbusopc
TEMPLATE =app

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
    ../../libmodbus/src/modbus-data.c \
    ../../libmodbus/src/modbus-rtu.c \
    ../../libmodbus/src/modbus-tcp.c \
    ../../libmodbus/src/modbus.c \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../../libmodbus/src/modbus-private.h \
    ../../libmodbus/src/modbus-rtu-private.h \
    ../../libmodbus/src/modbus-rtu.h \
    ../../libmodbus/src/modbus-tcp-private.h \
    ../../libmodbus/src/modbus-tcp.h \
    ../../libmodbus/src/modbus-version.h \
    ../../libmodbus/src/modbus.h \
    ../../libmodbus/src/win32/config.h \
    mainwindow.h \
    open62541.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../open62541/WS2_32/WS2_32.Lib \
    ../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug/createopc.dll



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug/ -lcreateopc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug/ -lcreateopcd
else:unix:!macx: LIBS += -L$$PWD/../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug/ -lcreateopc

INCLUDEPATH += $$PWD/../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug
DEPENDPATH += $$PWD/../build-createopc-Desktop_Qt_5_14_1_MinGW_64_bit-Debug
