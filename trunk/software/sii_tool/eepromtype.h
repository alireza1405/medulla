#ifndef EEPROMTYPE_H
#define EEPROMTYPE_H

#include <vector>
#include <stdint.h>
#include <QtXml>
#include <QDebug>

class eepromType
{
public:
    eepromType(QDomElement element);

    void setSize(uint16_t newSize);
    void setConfigData(uint16_t newConfigData[]);
    void setBootStrap(uint16_t newBootStrap[8]);

    uint16_t getSize();
    uint16_t* getConfigData();
    uint16_t* getBootStrap();
    bool hasDefinedData();

    std::vector<uint16_t> data;

private:
    uint16_t size; // Size of eeprom in bytes
    uint16_t configData[7]; // Configuration data
    uint16_t bootStrap[4]; // Bootstrap mailbox section data
    bool definedData;
};

#endif // EEPROMTYPE_H
