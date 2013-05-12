#-------------------------------------------------
#
# Project created by QtCreator 2013-05-11T22:16:31
#
#-------------------------------------------------

QT       += core xml

# greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG   += console
QT       -= gui

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
    syncmanagertype.h \
    sii_tool.h \
    pdotype.h \
    fmmutype.h \
    entrytype.h \
    eepromtype.h \
    devicetype.h \
    dctype.h \
    vendortype.h \
    sii_file.h

#FORMS    += \
#    sii_tool.ui

OTHER_FILES += \
    testESI.xml
