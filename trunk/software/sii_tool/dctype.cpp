#include "dctype.h"

dcType::dcType(QDomElement element, bool verbose)
{
    verb = verbose;
    QDomElement opMode;
    QDomNodeList nodes;
    QString attributeStr;

    // Find if there is an opMode specified
    nodes = element.elementsByTagName("OpMode");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found opMode Element, using first (default) element";

        opMode = nodes.at(0).toElement();
    }
    else
    {
        qDebug()<<"No opMode element found, ignoring dcType element";

        return;
    }

    // Now look for elements inside of the default opMode
    nodes = opMode.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found Name element:"<<nodes.at(0).toElement().text();

        DCSyncName = nodes.at(0).toElement().text();
    }

    nodes = opMode.elementsByTagName("CycleTimeSync0");
    cycleTimeSync0 = 0;
    cycleTimeSync0Factor = 0;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found CycleTimeSync0 element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            cycleTimeSync0 = indexStr.remove(0,2).toInt(0,16);
        else
            cycleTimeSync0 = indexStr.toInt(0,0);

        attributeStr = nodes.at(0).toElement().attribute("Factor");
        if (!attributeStr.isEmpty())
        {
            if (verb)
                qDebug()<<"---Found Factor with value:"<<attributeStr;
            // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
            if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
                cycleTimeSync0Factor = attributeStr.remove(0,2).toInt(0,16);
            else
                cycleTimeSync0Factor = attributeStr.toInt(0,0);
        }
    }

    nodes = opMode.elementsByTagName("CycleTimeSync1");
    cycleTimeSync1 = 0;
    cycleTimeSync1Factor = 0;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found CycleTimeSync1 element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            cycleTimeSync1 = indexStr.remove(0,2).toInt(0,16);
        else
            cycleTimeSync1 = indexStr.toInt(0,0);

        attributeStr = nodes.at(0).toElement().attribute("Factor");
        if (!attributeStr.isEmpty())
        {
            if (verb)
                qDebug()<<"---Found Factor with value:"<<attributeStr;
            // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
            if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
                cycleTimeSync1Factor = attributeStr.remove(0,2).toInt(0,16);
            else
                cycleTimeSync1Factor = attributeStr.toInt(0,0);
        }
    }

    nodes = opMode.elementsByTagName("ShiftTimeSync0");
    shiftTimeSync0 = 0;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found ShiftTimeSync0 element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            shiftTimeSync0 = indexStr.remove(0,2).toInt(0,16);
        else
            shiftTimeSync0 = indexStr.toInt(0,0);
    }

    nodes = opMode.elementsByTagName("ShiftTimeSync1");
    shiftTimeSync1 = 0;
    cycleTimeSync1Factor = 0;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found ShiftTimeSync1 element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            shiftTimeSync1 = indexStr.remove(0,2).toInt(0,16);
        else
            shiftTimeSync1 = indexStr.toInt(0,0);
    }

    nodes = opMode.elementsByTagName("AssignActivate");
    assignActivateWord = 0;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found AssignActivate element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            assignActivateWord = indexStr.remove(0,2).toInt(0,16);
        else
            assignActivateWord = indexStr.toInt(0,0);
    }

}
