#-------------------------------------------------
#
# Project created by QtCreator 2019-01-10T09:18:51
#
#-------------------------------------------------

QT       += core gui sql network charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = linearity_measurement
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
QMAKE_LFLAGS_RELEASE += -static -static-libgcc

CONFIG += c++14

SOURCES += main.cpp\
        gui.cpp \
    handler.cpp \
    analyzer.cpp \
    dialog.cpp \
    bd.cpp \
    channel.cpp \
    memory_list.cpp

HEADERS  += gui.h \
    handler.h \
    analyzer.h \
    dialog.h \
    bd.h \
    data.h \
    channel.h \
    memory_list.h

FORMS    += gui.ui \
    dialog.ui \
    bd.ui \
    memory_list.ui
