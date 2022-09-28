QT -= gui
QT += serialport


CONFIG += c++17 console
CONFIG -= app_bundle

#QMAKE_CXXFLAGS += -Wextra


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        devicemanager.cpp \
        main.cpp


HEADERS += \
    devicemanager.h
