#include "dctype.h"

dcType::dcType(QDomElement element)
{
    QDomElement opMode;
    QDomNodeList nodes;

    // Find if there is an opMode specified
    nodes = element.elementsByTagName("OpMode");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found opMode Element, using first (default) element";
        opMode = nodes.at(0).toElement();
    }
    else
    {
        qDebug()<<"No opMode element found, giving up";
        return;
    }

    // Now look for elements inside of the default opMode
    nodes = opMode.elementsByTagName("CycleTimeSync0");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found CycleTimeSync0 element:"<<nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            cycleTimeSync0 = indexStr.remove(0,2).toInt(0,16);
        else
            cycleTimeSync0 = indexStr.toInt(0,0);
    }

    nodes = opMode.elementsByTagName("ShiftTimeSync0");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found ShiftTimeSync0 element:"<<nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            shiftTimeSync0 = indexStr.remove(0,2).toInt(0,16);
        else
            shiftTimeSync0 = indexStr.toInt(0,0);
    }

    nodes = opMode.elementsByTagName("CycleTimeSync1");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found CycleTimeSync1 element:"<<nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            cycleTimeSync1 = indexStr.remove(0,2).toInt(0,16);
        else
            cycleTimeSync1 = indexStr.toInt(0,0);
    }

    nodes = opMode.elementsByTagName("ShiftTimeSync1");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found ShiftTimeSync1 element:"<<nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            shiftTimeSync1 = indexStr.remove(0,2).toInt(0,16);
        else
            shiftTimeSync1 = indexStr.toInt(0,0);
    }
}