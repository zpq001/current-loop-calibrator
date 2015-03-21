#-------------------------------------------------
#
# Project created by QtCreator 2015-03-13T09:36:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MenuSimulator
TEMPLATE = app
CONFIG += c++11

DEFINES += _MENU_SIMULATOR_

INCLUDEPATH += ../../source/inc/
INCLUDEPATH += ../../source/src/

SOURCES += main.cpp\
        mainwindow.cpp\
    keydriver.cpp \
    textdisplay.cpp \
    gui_wrapper.c \
    ../../source/src/gui_core.c \
    ../../source/src/gui_top.c \
    ../../source/src/utils.c \
    ../../source/src/linear_calibration.c

HEADERS  += mainwindow.h \
    keydriver.h \
    textdisplay.h \
    ../../source/inc/buttons.h \
    gui_wrapper.h \
    ../../source/inc/gui_core.h \
    ../../source/inc/gui_top.h \
    ../../source/inc/adc.h \
    ../../source/inc/dac.h \
    ../../source/inc/external_adc.h \
    ../../source/inc/led.h \
    ../../source/inc/linear_calibration.h \
    ../../source/inc/encoder.h

FORMS    += mainwindow.ui
