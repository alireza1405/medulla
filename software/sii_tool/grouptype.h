#ifndef GROUPTYPE_H
#define GROUPTYPE_H

#include <QString>
#include <stdint.h>
#include <QtXml>

class GroupType
{
public:
    GroupType(QDomElement element, bool verbose);

    QString type;
    QString name;

private:
    bool verb;
};

#endif // GROUPTYPE_H
