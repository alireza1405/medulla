#ifndef FMMUTYPE_H
#define FMMUTYPE_H

#include <QtXml>
#include <stdint.h>

class fmmuType
{
public:
    fmmuType(QDomElement element);

    enum fmmuType_t : uint8_t {
        unused = 0x00,
        processDataOut = 0x03,
        processDataIn = 0x04,
        syncMStatus = 0x03};

    fmmuType_t type;
    uint8_t sm;

};

#endif // FMMUTYPE_H
