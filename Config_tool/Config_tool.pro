#-------------------------------------------------
#
# Project created by QtCreator 2017-12-17T17:39:10
#
#-------------------------------------------------

QT       += core gui
QT       +=serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Config_tool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    communictioanframe.h

FORMS    += mainwindow.ui
