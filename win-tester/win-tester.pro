QT      += core gui
QT      += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    controller.cpp \
    deviceliga.cpp \
#    init.cpp \
#    loop.cpp \
    main.cpp \
    mainwindow.cpp \
#    modbus.cpp \
#    objects.cpp \
#    sensor.cpp \
    serialport.cpp
#    stepper.cpp

HEADERS += \
    deviceliga.h \
    mainwindow.h \
    serialport.h
#    loop.h \
#    sensor.h \
#    modbus.h \
#    stepper.h \
#    objects.h \
#    controller.h \
#    init.h

FORMS += \
    mainwindow.ui

