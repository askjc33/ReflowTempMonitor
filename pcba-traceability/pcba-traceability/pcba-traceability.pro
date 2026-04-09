QT += core gui widgets serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = PCBATraceability

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

win32:CONFIG(release, debug|release): CONFIG += windows
