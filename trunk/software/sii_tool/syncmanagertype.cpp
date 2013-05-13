#include "syncmanagertype.h"

syncManagerType::syncManagerType(QDomElement element, bool verbose)
{
    QString attributeStr;
    verb = verbose;

    // Parse value for SM type
    if (element.text().compare("MBoxOut") == 0)
    {
        if (verb)
            qDebug()<<"--SM type: MBoxOut";
        smType = mailboxOut;
    }
    else if (element.text().compare("MBoxIn") == 0)
    {
        if (verb)
            qDebug()<<"--SM type: MBoxIn";
        smType = mailboxIn;
    }
    else if (element.text().compare("Outputs") == 0)
    {
        if (verb)
            qDebug()<<"--SM type: Outputs";
        smType = processDataOut;
    }
    else if (element.text().compare("Inputs") == 0)
    {
        if (verb)
            qDebug()<<"--SM type: Inputs";
        smType = processDataIn;
    }
    else
    {
        if (verb)
            qDebug()<<"--SM type: Unused";
        smType = unused;
        phyStartAddr = 0;
        length = 0;
        controlRegister = 0;
        enable = false;
        virtualSM = false;
        opOnly = false;
        return;
    }

    attributeStr.clear();
    attributeStr = element.attribute("StartAddress");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found start address with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            phyStartAddr = attributeStr.remove(0,2).toInt(0,16);
        else
            phyStartAddr = attributeStr.toInt(0,0);
    }
    else
        qFatal("No start address found for sync manager");

    attributeStr.clear();
    attributeStr = element.attribute("ControlByte");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found ControlByte with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            controlRegister = attributeStr.remove(0,2).toInt(0,16);
        else
            controlRegister = attributeStr.toInt(0,0);
    }
    else
        controlRegister = 0;

    attributeStr.clear();
    attributeStr = element.attribute("DefaultSize");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found DefaultSize with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            length = attributeStr.remove(0,2).toInt(0,16);
        else
            length = attributeStr.toInt(0,0);
    }
    else
        length = 0;

    attributeStr.clear();
    attributeStr = element.attribute("Enable");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found Enable attribute with value:"<<attributeStr;
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            enable = attributeStr.remove(0,2).toInt(0,16) != 0;
        else
            enable = attributeStr.toInt(0,0) != 0;
    }
    else
        enable = false;

    attributeStr.clear();
    attributeStr = element.attribute("Virtual");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found Virtual attribute with value:"<<attributeStr;
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            virtualSM = attributeStr.remove(0,2).toInt(0,16) != 0;
        else
            virtualSM = attributeStr.toInt(0,0) != 0;
    }
    else
        virtualSM = false;

    attributeStr.clear();
    attributeStr = element.attribute("OpOnly");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found OpOnly attribute with value:"<<attributeStr;
        opOnly = (attributeStr.remove(0,2).toInt(0,16) != 0);
    }
    else
        opOnly = false; 

}
