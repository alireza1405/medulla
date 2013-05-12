#include "pdotype.h"

pdoType::pdoType(QDomElement element, bool verbose)
{
    QDomNodeList nodes;
    verb = verbose;

    // Parse Name element if there is one
    nodes = element.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Name element:"<<nodes.at(0).toElement().text();;
        name = nodes.at(0).toElement().text();
    }

    // Parse Index element if there is one
    nodes = element.elementsByTagName("Index");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Index element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            index = indexStr.remove(0,2).toInt(0,16);
        else
            index = indexStr.toInt(0,0);
    }
    else
        qFatal("Couldn't Find PDO index");

    // Parse PDO Entry list
    nodes = element.elementsByTagName("Entry");
    for (int entry = 0; entry < nodes.count(); entry++)
    {
        if (verb)
            qDebug()<<"Parsing PDO entry"<<entry;
        pdoEntries.append(EntryType(nodes.at(entry).toElement(),verb));
    }

    // Parse attributes
    fixed = element.attribute("Fixed","0").at(0) != '0';
    mandatory = element.attribute("Mandatory","0").at(0) != '0';
    if (element.attribute("Sm","-1").toInt() >= 0)
        syncManager = element.attribute("Sm","-1").toInt();
    else
        qFatal("No assigned Sync Manager");
}
