#ifndef DCTYPE_H
#define DCTYPE_H

#include <QtXml>
#include <stdint.h>
#include <QList>
#include <QString>

class dcType
{
public:
    dcType(QDomElement element, bool verbose);

    uint32_t cycleTimeSync0;
    uint32_t shiftTimeSync0;
    uint32_t shiftTimeSync1;
    int16_t cycleTimeSync0Factor;
    int16_t cycleTimeSync1Factor;
    uint16_t assignActivateWord;
    QString DCSyncName;

private:
    bool verb;
};

#endif // DCTYPE_H
