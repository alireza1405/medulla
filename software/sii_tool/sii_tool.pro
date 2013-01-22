#-------------------------------------------------
#
# Project created by QtCreator 2013-01-20T12:50:21
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sii_tool
TEMPLATE = app

QMAKE_CXXFLAGS += -std=gnu++11


SOURCES += main.cpp \
    vendortype.cpp \
    syncmanagertype.cpp \
    sii_tool.cpp \
    pdotype.cpp \
    fmmutype.cpp \
    entrytype.cpp \
    eepromtype.cpp \
    devicetype.cpp \
    dctype.cpp

HEADERS  += \
    vendortype.h \
    syncmanagertype.h \
    sii_tool.h \
    pdotype.h \
    fmmutype.h \
    entrytype.h \
    eepromtype.h \
    devicetype.h \
    dctype.h

FORMS    += \
    sii_tool.ui
