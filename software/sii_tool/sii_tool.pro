#-------------------------------------------------
#
# Project created by QtCreator 2013-01-04T22:24:56
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sii_tool
TEMPLATE = app


SOURCES += main.cpp\
        sii_tool.cpp \
    vendortype.cpp \
    eepromtype.cpp \
    devicetype.cpp \
    entrytype.cpp \
    pdotype.cpp \
    syncmanagertype.cpp \
    fmmutype.cpp \
    dctype.cpp

HEADERS  += sii_tool.h \
    vendortype.h \
    eepromtype.h \
    devicetype.h \
    entrytype.h \
    pdotype.h \
    syncmanagertype.h \
    fmmutype.h \
    dctype.h

FORMS    += sii_tool.ui

QMAKE_CXXFLAGS += -std=gnu++11
