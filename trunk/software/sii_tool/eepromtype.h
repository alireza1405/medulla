#ifndef EEPROMTYPE_H
#define EEPROMTYPE_H

#include <vector>
#include <stdint.h>
#include <QtXml>
#include <QDebug>
#include <QByteArray>

class eepromType
{
public:
    eepromType(QDomElement element);

    uint16_t size; // Size of eeprom in bytes
    QByteArray configData; // Configuration data
    QByteArray bootStrap; // Bootstrap mailbox section data
    QByteArray eepromData; // Full eeprom data stored here if it's in the ESI file.
    QByteArray siiData;
    bool definedData;
};

#endif // EEPROMTYPE_H
