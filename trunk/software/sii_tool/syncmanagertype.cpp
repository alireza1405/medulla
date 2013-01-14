#include "syncmanagertype.h"

syncManagerType::syncManagerType(QDomElement element)
{
    QString attributeStr;

    // Parse value for SM type
    if (element.text().compare("MBoxOut") == 0)
        smType = mailboxOut;
    else if (element.text().compare("MBoxIn") == 0)
        smType = mailboxIn;
    else if (element.text().compare("Outputs") == 0)
        smType = processDataOut;
    else if (element.text().compare("Inputs") == 0)
        smType = processDataIn;
    else
        smType = unused;

    attributeStr = element.attribute("StartAddress");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found start address with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            phyStartAddr = attributeStr.remove(0,2).toInt(0,16);
        else
            phyStartAddr = attributeStr.toInt(0,0);
    }
    else
        qErrnoWarning("No start address found for sync manager");

    attributeStr = element.attribute("ControlByte");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found ControlByte with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            controlRegister = attributeStr.remove(0,2).toInt(0,16);
        else
            controlRegister = attributeStr.toInt(0,0);
    }
    else
        controlRegister = 0;

    attributeStr = element.attribute("DefaultSize");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found DefaultSize with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            length = attributeStr.remove(0,2).toInt(0,16);
        else
            length = attributeStr.toInt(0,0);
    }
    else
        length = 0;

    attributeStr = element.attribute("ControlByte");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found Enable attribute with value:"<<attributeStr;
            enable = (attributeStr.remove(0,2).toInt(0,16) != 0);
    }
    else
        enable = false;

    attributeStr = element.attribute("Virtual");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found Virtual attribute with value:"<<attributeStr;
            virtualSM = (attributeStr.remove(0,2).toInt(0,16) != 0);
    }
    else
        virtualSM = false;

    attributeStr = element.attribute("OpOnly");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found OpOnly attribute with value:"<<attributeStr;
            opOnly = (attributeStr.remove(0,2).toInt(0,16) != 0);
    }
    else
        opOnly = false;
}
