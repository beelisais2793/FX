#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T19:33:58
#
#-------------------------------------------------

QT       += core gui opengl printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += opengl widgets

TARGET = pend2
TEMPLATE = app

CONFIG += c++11 staticlib

SOURCES += main.cpp\
        mainwindow.cpp \
    annotation.cpp \
    phyzxmodel.cpp \
    globj.cpp \
    scene.cpp \
    qcustomplot.cpp \
    graphs.cpp \
    rungekutta4.cpp \
    timer.cpp

HEADERS  += mainwindow.h \
    annotation.h \
    phyzxmodel.h \
    globj.h \
    scene.h \
    qcustomplot.h \
    graphs.h \
    rungekutta4.h \
    timer.h

FORMS    += mainwindow.ui \
    annotation.ui \
    graphs.ui

RESOURCES += \
    ares.qrc \
    models.qrc

win32{
LIBS += opengl32.lib
}
