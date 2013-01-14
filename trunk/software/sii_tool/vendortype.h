#ifndef VENDORTYPE_H
#define VENDORTYPE_H

#include <QString>
#include <stdint.h>
#include <QtXml>

class VendorType
{
public:
    VendorType(QDomElement element);

    uint32_t vendorID;
    QString name;
};

#endif // VENDORTYPE_H
