#include "grouptype.h"

GroupType::GroupType(QDomElement element, bool verbose)
{
    QDomNodeList nodes;
    verb = verbose;

    // Parse Type element if there is one
    nodes = element.elementsByTagName("Type");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"-Found Type element:"<<nodes.at(0).toElement().text();;
        type = nodes.at(0).toElement().text();
    }
    else
        qFatal("No group type specified");

    // Parse Name element if there is one
    nodes = element.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"-Found Name element:"<<nodes.at(0).toElement().text();;
        name = nodes.at(0).toElement().text();
    }
}
