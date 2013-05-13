#ifndef DEVICETYPE_H
#define DEVICETYPE_H

#include <QtXml>
#include <stdint.h>
#include <QString>
#include <QList>
#include <QDebug>

#include "fmmutype.h"
#include "syncmanagertype.h"
#include "pdotype.h"
#include "dctype.h"
#include "eepromtype.h"
#include "vendortype.h"
#include "grouptype.h"

class DeviceType
{
public:
    DeviceType(QDomElement element, VendorType *deviceVendor, QList<GroupType> groupList, bool verbose);

    enum physicsType {
        notUsed = 0x00,
        MII = 0x01,
        EBUS = 0x02
    };

    void parseType(QDomElement element);
    void write_sii();

    uint32_t productCode;
    uint32_t revisionNumber;
    uint32_t serialNumber;
    uint16_t mailboxProtocol;
    int groupId;
    QByteArray image;
    QString type;
    QString name;
    bool enableSDO;
    bool enableSDOInfo;
    bool enablePDOAssign;
    bool enablePDOConfig;
    bool enableUploadAtStartup;
    bool enableSDOCompleteAccess;
    bool enableFoE;
    bool enableEoE;
    bool enableSafeOp;
    bool enableenableNotLRW;
    int16_t ebusCurrent;
    physicsType physics[4];
    QList<fmmuType> fmmus;
    QList<syncManagerType> syncManagers;
    QList<pdoType> txPDOs;
    QList<pdoType> rxPDOs;
    dcType *dcConf;
    eepromType *eeprom;
    VendorType *vendor;

private:
    bool verb;
};

#endif // DEVICETYPE_H
