QT += core gui widgets serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = ReflowTempMonitor
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    serialworker.cpp \
    pcbdatamanager.cpp \
    temperaturechart.cpp \
    chambervisualizer.cpp

HEADERS += \
    mainwindow.h \
    serialworker.h \
    pcbdatamanager.h \
    temperaturechart.h \
    chambervisualizer.h
