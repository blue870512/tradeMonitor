#-------------------------------------------------
#
# Project created by QtCreator 2017-02-28T09:49:23
#
#-------------------------------------------------

QT       += core gui sql xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tradeMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    opening.cpp \
    common.cpp \
    db_handle.cpp \
    detail_thread.cpp \
    export_file.cpp \
    info_table.cpp \
    settings.cpp \
    status_table.cpp \
    tm_log.cpp \
    tm_thread.cpp \
    closing.cpp \
    tm_conf.cpp \
    global.cpp \
    sqlsettings.cpp \
    udp_client.cpp

HEADERS  += mainwindow.h \
    opening.h \
    common.h \
    db_handle.h \
    detail_thread.h \
    export_file.h \
    info_table.h \
    settings.h \
    status_table.h \
    tm_log.h \
    tm_thread.h \
    closing.h \
    tm_conf.h \
    global.h \
    sqlsettings.h \
    udp_client.h

FORMS    += mainwindow.ui \
    export_file.ui \
    settings.ui \
    sqlsettings.ui

RESOURCES += \
    icon.qrc

RC_FILE = tradeMonitor.rc
