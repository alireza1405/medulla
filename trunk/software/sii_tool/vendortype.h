#ifndef VENDORTYPE_H
#define VENDORTYPE_H

#include <QString>
#include <stdint.h>
#include <QtXml>

class VendorType
{
public:
    VendorType(QDomElement element, bool verbose);

    uint32_t vendorID;
    QString name;

private:
    bool verb;
};

#endif // VENDORTYPE_H
