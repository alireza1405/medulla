#include "fmmutype.h"

fmmuType::fmmuType(QDomElement element, bool verbose)
{
    QString attributeStr;
    verb = verbose;

    // Parse value for fmmu type
    if (element.text().compare("Outputs") == 0)
    {
        type = processDataOut;
        if (verb)
            qDebug()<<"--FMMU type: Outputs";
    }
    else if (element.text().compare("Inputs") == 0)
    {
        type = processDataIn;
        if (verb)
            qDebug()<<"--FMMU type: Inputs";
    }
    else if (element.text().compare("MBoxState") == 0)
    {
        type = syncMStatus;
        if (verb)
            qDebug()<<"--FMMU type: MBoxState";
    }
    else
        type = unused;

    attributeStr = element.attribute("Sm");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found Sm attribute with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            sm = attributeStr.remove(0,2).toInt(0,16);
        else
            sm = attributeStr.toInt(0,0);
        smAssigned = true;
    }
    else
    {
        smAssigned = false;
        sm = 0;
    }

    attributeStr = element.attribute("Su");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found Su attribute with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            su = attributeStr.remove(0,2).toInt(0,16);
        else
            su = attributeStr.toInt(0,0);
        suAssigned = true;
    }
    else
    {
        suAssigned = false;
        su = 0;
    }

    // We need to chatch this to be complient with outdated esi files. (Or poorly written ones.)
    attributeStr.clear();
    attributeStr = element.attribute("OpOnly");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found OpOnly attribute with value:"<<attributeStr;
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            opOnly = attributeStr.remove(0,2).toInt(0,16) != 0;
        else
            opOnly = attributeStr.toInt(0,0) != 0;
    }
    else
        opOnly = false;
}
