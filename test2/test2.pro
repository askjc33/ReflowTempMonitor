QT += core gui widgets serialport charts

CONFIG += c++17

TEMPLATE = app
TARGET = test2

SOURCES += \
    main.cpp \
    sources/serial_manager.cpp \
    sources/board_trace_manager.cpp \
    sources/connection_page.cpp \
    sources/traceability_page.cpp \
    sources/main_window.cpp

HEADERS += \
    headers/board_record.h \
    headers/serial_manager.h \
    headers/board_trace_manager.h \
    headers/connection_page.h \
    headers/traceability_page.h \
    headers/main_window.h

INCLUDEPATH += headers

# 可选：Windows 下隐藏控制台
win32:CONFIG(release, debug|release): CONFIG += windows
