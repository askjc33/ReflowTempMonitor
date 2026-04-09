QT += core gui widgets serialport charts

CONFIG += c++11

TARGET = ReflowMonitor
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    serialmanager.cpp \
    connectionpage.cpp \
    presetpage.cpp \
    datamonitorpage.cpp

HEADERS += \
    mainwindow.h \
    serialmanager.h \
    connectionpage.h \
    presetpage.h \
    datamonitorpage.h
