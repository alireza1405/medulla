#ifndef FMMUTYPE_H
#define FMMUTYPE_H

#include <QtXml>
#include <stdint.h>

class fmmuType
{
public:
    fmmuType(QDomElement element, bool verbose);

    enum fmmuType_t {
        unused = 0x00,
        processDataOut = 0x01,
        processDataIn = 0x02,
        syncMStatus = 0x03};

    fmmuType_t type;
    uint8_t sm;
    uint8_t su;
    bool smAssigned;
    bool suAssigned;
    bool opOnly;

private:
    bool verb;

};

#endif // FMMUTYPE_H
