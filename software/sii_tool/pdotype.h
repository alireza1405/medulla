#ifndef PDOTYPE_H
#define PDOTYPE_H

#include <QtXml>
#include <stdint.h>
#include <QList>

#include <entrytype.h>

class pdoType
{
public:
    pdoType(QDomElement element, bool verbose);

    uint16_t index;
    uint8_t syncManager;
    uint8_t synchronization;
    QString name;
    bool fixed;
    bool mandatory;

    QList<EntryType> pdoEntries;

private:
    bool verb;
};

#endif // PDOTYPE_H
