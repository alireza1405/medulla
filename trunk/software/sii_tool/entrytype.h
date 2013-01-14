#ifndef ENTRYTYPE_H
#define ENTRYTYPE_H

#include <QtXml>
#include <stdint.h>

class EntryType
{
public:
    EntryType(QDomElement element);

    uint16_t index;
    uint8_t subIndex;
    QString name;
    uint8_t typeIndex; // Index of type in COE dictionary
    uint8_t bitLen;
};

#endif // ENTRYTYPE_H
