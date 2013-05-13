#include "devicetype.h"

DeviceType::DeviceType(QDomElement element, VendorType *deviceVendor, QList<GroupType> groupList, bool verbose)
{
    verb = verbose;
    vendor = deviceVendor;
    // Parse Physics attribute, not technically documented, but it seems to work and be used
    QString attributeStr;

    //TODO: Parse all of these values out of ESI file
    enableSDO = false;
    enableSDOInfo = false;
    enablePDOAssign = false;
    enablePDOConfig = false;
    enableUploadAtStartup = false;
    enableSDOCompleteAccess = false;
    enableFoE = false;
    enableEoE = false;
    enableSafeOp = false;
    enableenableNotLRW = false;
    ebusCurrent = 0;

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
                    qDebug()<<"-Port"<<i<<"using MII physics.";
            }
            else if (attributeStr.at(i) == 'K')
            {
                physics[i] = EBUS;
                if (verb)
                    qDebug()<<"-Port"<<i<<"using EBUS physics.";
            }
        }
    }

    QDomNodeList nodes;

    // Parse Type element if there is one
    nodes = element.elementsByTagName("Type");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"-Found Type element:"<<nodes.at(0).toElement().text();
        parseType(nodes.at(0).toElement());
    }
    else
        qFatal("Couldn't find Type element");

    // Parse Name element
    nodes = element.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"--Found Name element:"<<nodes.at(0).toElement().text();
        name = nodes.at(0).toElement().text();
    }

    // Parse GroupType element
    nodes = element.elementsByTagName("GroupType");
    groupId = -1;
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"-Found GroupType element:"<<nodes.at(0).toElement().text();

        // Try to find the group in the group list
        for (int count = 0; count < groupList.count(); count++)
        {
            if (groupList[count].type == nodes.at(0).toElement().text())
                groupId = count;
        }

        if (groupId != -1)
        {
            if (verb)
                qDebug()<<"--Found group with name"<<groupList[groupId].name;
        }
        else
            qFatal("Failed to find group in group list");
    }
    else
        qFatal("Couldn't find GroupType element");

    // Parse Fmmu elements
    nodes = element.elementsByTagName("Fmmu");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"-Processing Fmmu element"<<node;
        fmmus.append(fmmuType(nodes.at(node).toElement(),verb));
    }

    // Parse Sm elements
    nodes = element.elementsByTagName("Sm");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"-Processing Sm element"<<node;
        syncManagers.append(syncManagerType(nodes.at(node).toElement(),verb));
    }

    // Handle misplaced OpOnly elements
    for (int count = 0; count < fmmus.count(); count++)
    {
        if (fmmus[count].smAssigned && (fmmus[count].sm < syncManagers.count()) && fmmus[count].opOnly)
        {
            qDebug()<< "Found obsolete use of OpOnly attribute of FMMU Type";
            if (syncManagers[fmmus[count].sm].smType != syncManagerType::processDataIn) // For some reason TwinCAT does this
                syncManagers[fmmus[count].sm].opOnly = true;
        }
    }

    // Parse RxPDO element
    nodes = element.elementsByTagName("RxPdo");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"-Processing RxPDO element"<<node;
        rxPDOs.append(pdoType(nodes.at(node).toElement(),verb));
    }

    // Parse TxPDO element
    nodes = element.elementsByTagName("TxPdo");
    for (unsigned int node = 0; node < nodes.length(); node++)
    {
        if (verb)
            qDebug()<<"-Processing TxPDO element"<<node;
        txPDOs.append(pdoType(nodes.at(node).toElement(),verb));
    }

    // Parse DC element if there is one
    dcConf = 0;
    nodes = element.elementsByTagName("Dc");
    if (nodes.count()>0)
    {
        if (verb)
            qDebug()<<"-Found DC element";
        dcConf = new dcType(nodes.at(0).toElement(),verb);
    }

    // Parse EEPROM element
    eeprom = 0;
    nodes = element.elementsByTagName("Eeprom");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"-Found Eeprom element";
        eeprom = new eepromType(nodes.at(0).toElement(),verb);
    }
    else
        qFatal("Couldn't find EEPROM element");
}

void DeviceType::parseType(QDomElement element) {
    QString attributeStr;

    attributeStr.clear();
    attributeStr = element.attribute("ProductCode");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found ProductCode with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            productCode = attributeStr.remove(0,2).toInt(0,16);
        else
            productCode = attributeStr.toInt(0,0);
    }
    else
        productCode = 0;

    attributeStr.clear();
    attributeStr = element.attribute("RevisionNo");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found RevisionNo with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            revisionNumber = attributeStr.remove(0,2).toInt(0,16);
        else
            revisionNumber = attributeStr.toInt(0,0);
    }
    else
        revisionNumber = 0;

    attributeStr.clear();
    attributeStr = element.attribute("SerialNo");
    if (!attributeStr.isEmpty())
    {
        if (verb)
            qDebug()<<"--Found SerialNo with value:"<<attributeStr;
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((attributeStr.at(0) == '#') && (attributeStr.at(1) == 'x'))
            serialNumber = attributeStr.remove(0,2).toInt(0,16);
        else
            serialNumber = attributeStr.toInt(0,0);
    }
    else
        serialNumber = 0;

    type = element.text();
}

void DeviceType::write_sii() {
    QByteArray fileArray;

}
