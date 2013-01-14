#include "fmmutype.h"

fmmuType::fmmuType(QDomElement element)
{
    QString attributeStr;

    // Parse value for fmmu type
    if (element.text().compare("Outputs") == 0)
        type = processDataOut;
    else if (element.text().compare("Inputs") == 0)
        type = processDataIn;
    else if (element.text().compare("MBoxState") == 0)
        type = syncMStatus;
    else
        type = unused;

    attributeStr = element.attribute("Sm");
    if (!attributeStr.isEmpty())
    {
        qDebug()<<"Found Sm attribute with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            sm = attributeStr.remove(0,2).toInt(0,16);
        else
            sm = attributeStr.toInt(0,0);
    }
    else
        sm = 0;
}
