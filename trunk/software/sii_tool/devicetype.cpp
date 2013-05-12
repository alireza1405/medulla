#include "devicetype.h"

DeviceType::DeviceType(QDomElement element, VendorType *deviceVendor, bool verbose)
{
    verb = verbose;
    vendor = deviceVendor;
    // Parse Physics attribute, not technically documented, but it seems to work and be used
    QString attributeStr;

    physics[0] = notUsed;
    physics[1] = notUsed;
    physics[2] = notUsed;
    physics[3] = notUsed;
    attributeStr = element.attribute("Physics");
    if (!attributeStr.isEmpty())
    {
        for (int i = 0; i < attributeStr.length(); i++)
        {
            if (i >= 4)
                break;
            if ((attributeStr.at(i) == 'Y') || (attributeStr.at(i) == 'H')) {
                physics[i] = MII;
                if (verb)
                    qDebug()<<"Port"<<i<<"using MII physics.";
            }
            else if (attributeStr.at(i) == 'K')
            {
                physics[i] = EBUS;
                if (verb)
                    qDebug()<<"Port"<<i<<"using EBUS physics.";
            }
        }
    }

    QDomNodeList nodes;

    // Parse Type element if there is one
    nodes = element.elementsByTagName("Type");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Type element:"<<nodes.at(0).toElement().text();
        parseType(nodes.at(0).toElement());
    }
    else
        qFatal("Couldn't find Type element");

    // Parse GroupType element
    nodes = element.elementsByTagName("GroupType");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found GroupType element:"<<nodes.at(0).toElement().text();
        groupName = nodes.at(0).toElement().text();
    }
    else
        qFatal("Couldn't find GroupType element");

    // Parse Fmmu elements
    nodes = element.elementsByTagName("Fmmu");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"Processing Fmmu element"<<node;
        fmmus.append(fmmuType(nodes.at(node).toElement(),verb));
    }

    // Parse Sm elements
    nodes = element.elementsByTagName("Sm");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"Processing Sm element"<<node;
        syncManagers.append(syncManagerType(nodes.at(node).toElement(),verb));
    }

    // Parse RxPDO element
    nodes = element.elementsByTagName("RxPdo");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"Processing RxPDO element"<<node;
        rxPDOs.append(pdoType(nodes.at(node).toElement(),verb));
    }

    // Parse TxPDO element
    nodes = element.elementsByTagName("TxPdo");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"Processing TxPDO element"<<node;
        txPDOs.append(pdoType(nodes.at(node).toElement(),verb));
    }

    // Parse DC element if there is one
    dcConf = 0;
    nodes = element.elementsByTagName("Dc");
    if (nodes.count()>0)
    {
        if (verb)
            qDebug()<<"Found DC element";
        dcConf = new dcType(nodes.at(0).toElement(),verb);
    }

    // Parse EEPROM element
    eeprom = 0;
    nodes = element.elementsByTagName("Eeprom");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Eeprom element";
        eeprom = new eepromType(nodes.at(0).toElement(),verb);
    }
    else
        qFatal("Couldn't find EEPROM element");
}

void DeviceType::parseType(QDomElement element) {
    QString attributeStr;

    attributeStr = element.attribute("ProductCode");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"Found ProductCode with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            productCode = attributeStr.remove(0,2).toInt(0,16);
        else
            productCode = attributeStr.toInt(0,0);
    }
    else
        productCode = 0;

    attributeStr = element.attribute("RevisionNo");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"Found RevisionNo with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            revisionNumber = attributeStr.remove(0,2).toInt(0,16);
        else
            revisionNumber = attributeStr.toInt(0,0);
    }
    else
        revisionNumber = 0;

    attributeStr = element.attribute("SerialNo");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"Found SerialNo with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            serialNumber = attributeStr.remove(0,2).toInt(0,16);
        else
            serialNumber = attributeStr.toInt(0,0);
    }
    else
        serialNumber = 0;

    name = element.text();
}

void DeviceType::write_sii() {
    QByteArray fileArray;

}
