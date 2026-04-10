QT += core gui widgets serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TEMPLATE = app
TARGET = test3

DEFINES += QT_DEPRECATED_WARNINGS

# ===== 新增：温区设置页面与温区配置模块已加入工程 =====

SOURCES += \
    board_trace_manager.cpp \
    connection_page.cpp \
    main.cpp \
    main_window.cpp \
    reflow_settings.cpp \
    serial_manager.cpp \
    traceability_page.cpp \
    zone_settings_page.cpp

HEADERS += \
    board_record.h \
    board_trace_manager.h \
    connection_page.h \
    main_window.h \
    reflow_settings.h \
    serial_manager.h \
    traceability_page.h \
    zone_settings_page.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
