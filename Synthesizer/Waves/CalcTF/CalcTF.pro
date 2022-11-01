#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T09:50:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport concurrent

#QT += webenginewidgets webchannel
CONFIG +=C++11


TARGET = CalcTF
TEMPLATE = app


SOURCES += main.cpp\
    Polynomial.cpp \
    PolynomialRootFinder.cpp \
    tfdialog.cpp \
    transferfunction.cpp \
    controlsystem.cpp \
    errorstrings.cpp \
    outputdialog.cpp \
    root.cpp \
    roottablemodel.cpp \
    jshelper.cpp \
    plotdialog.cpp \
    qcustomplot.cpp \
    customgraph.cpp \
    rootlocusgraph.cpp \
    rootlocus.cpp \
    intervaltf.cpp \
    tfcoefficient.cpp \
    toolbox.cpp

HEADERS  += \
    Polynomial.h \
    PolynomialRootFinder.h \
    tfdialog.h \
    transferfunction.h \
    controlsystem.h \
    errorstrings.h \
    outputdialog.h \
    root.h \
    roottablemodel.h \
    jshelper.h \
    plotdialog.h \
    qcustomplot.h \
    customgraph.h \
    rootlocusgraph.h \
    rootlocus.h \
    intervaltf.h \
    tfcoefficient.h \
    toolbox.h

FORMS    += \
    tfdialog.ui \
    outputdialog.ui \
    plotdialog.ui

RESOURCES += \
    calctfres.qrc

DISTFILES += \
    screen.html
