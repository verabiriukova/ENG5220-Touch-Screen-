# Qt project file - qmake uses his to generate a Makefile
#blah

QT       += core gui

CONFIG          += qt warn_on debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QwtExample

LIBS += -lqwt -lm

HEADERS += window.h adcreader.h gpio-sysfs.h

SOURCES += main.cpp window.cpp adcreader.cpp gpio-sysfs.cpp
