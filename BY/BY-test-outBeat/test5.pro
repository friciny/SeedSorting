#-------------------------------------------------
#
# Project created by QtCreator 2014-03-14T21:02:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sensor.cpp \
    qextserialbase.cpp \
    posix_qextserialport.cpp \
    screen.cpp \
    timecount.cpp \
    actor.cpp

HEADERS  += mainwindow.h \
    global.h \
    sensor.h \
    qextserialbase.h \
    posix_qextserialport.h \
    screen.h \
    timecount.h \
    actor.h
