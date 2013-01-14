#ifndef DEVICETYPE_H
#define DEVICETYPE_H

#include <QtXml>
#include <stdint.h>
#include <QString>

class DeviceType
{
public:
    DeviceType(QDomElement element);

    enum physicsType {
        notUsed = 0x00,
        MII = 0x01,
        EBUS = 0x02
    };

    uint32_t productCode;
    uint32_t revisionNumber;
    uint32_t serialNumber;
    uint16_t mailboxProtocol;
    QString groupName;
    QByteArray image;
    QString order;
    QString name;
    bool enableSDO;
    bool enableSDOInfo;
    bool enablePDOAssign;
    bool enalbePDOConfig;
    bool enableUploadAtStartup;
    bool enableSDOCompleteAccess;
    bool enableFoE;
    bool enableEoE;
    bool enableSafeOp;
    bool enableenableNotLRW;
    uint16_t ebusCurrent;
    physicsType physics[4];
};

#endif // DEVICETYPE_H
