#ifndef SYNCMANAGERTYPE_H
#define SYNCMANAGERTYPE_H

#include <QtXml>
#include <stdint.h>

class syncManagerType
{
public:
    syncManagerType(QDomElement element, bool verbose);

    enum smType_t : uint8_t {
        unused = 0x00,
        mailboxOut = 0x01,
        mailboxIn = 0x02,
        processDataOut = 0x03,
        processDataIn = 0x04};

    uint16_t phyStartAddr;
    uint16_t length;
    uint8_t controlRegister;
    bool enable;
    bool virtualSM;
    bool opOnly;
    smType_t smType;

private:
    bool verb;
};

#endif // SYNCMANAGERTYPE_H
