#include "vendortype.h"

VendorType::VendorType(QDomElement element, bool verbose)
{
    QDomNodeList nodes;
    verb = verbose;

    // Parse Id element if there is one
    nodes = element.elementsByTagName("Id");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Id element:"<<nodes.at(0).toElement().text();
        QString idStr = nodes.at(0).toElement().text();
        if ((idStr.at(0) == '#') && (idStr.at(1) == 'x')) // Catch strange XML hex encoding
            vendorID = (idStr.remove(0,2).toInt(0,16));
        else
            vendorID = (idStr.toInt(0,0));
    }
    else
        qFatal("No vendor ID found");

    // Parse Name element if there is one
    nodes = element.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Name element:"<<nodes.at(0).toElement().text();;
        name = nodes.at(0).toElement().text();
    }
}
