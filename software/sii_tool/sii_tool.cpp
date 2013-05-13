#include "sii_tool.h"

sii_tool::sii_tool(bool verb)
{
    vendor = 0;
    verbose = verb;
}

sii_tool::~sii_tool()
{
    delete vendor;
}

void sii_tool::parse_esi_file(QString filename)
{
    QDomDocument doc("ESIFile");
    QFile file(filename);

    qDebug()<<"Parsing ESI file";

    if (!file.open(QIODevice::ReadOnly)) {
        qFatal("Couldn't Open file.");
    }

    if (!doc.setContent(&file)) {
        file.close();
        qFatal("Failed to read file");
    }
    file.close();

    if (verbose)
        qDebug()<<"------Parsing vendor element------";
    vendor = new VendorType(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Vendor").at(0).toElement(),verbose);

    if (verbose)
        qDebug()<<"------Parsing group elements------";
    groups.clear();
    QDomNodeList groupsElements = doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Descriptions").at(0).toElement().elementsByTagName("Groups");
    for (int groupsCounter = 0; groupsCounter < groupsElements.count(); groupsCounter++)
    {
        QDomNodeList groupElements = groupsElements.at(groupsCounter).toElement().elementsByTagName("Group");
        for (int groupCounter = 0; groupCounter < groupElements.count(); groupCounter++) {
            if (verbose)
                qDebug() << "Parsing group element:";
            groups.append(GroupType(groupElements.at(groupCounter).toElement(),verbose));
        }
    }

    if (verbose)
        qDebug()<<"------Parsing device elements------";
    devices.clear();
    QDomNodeList devicesElements = doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Descriptions").at(0).toElement().elementsByTagName("Devices");
    for (int devicesCounter = 0; devicesCounter < devicesElements.count(); devicesCounter++)
    {
        QDomNodeList deviceElements = devicesElements.at(devicesCounter).toElement().elementsByTagName("Device");
        for (int deviceCounter = 0; deviceCounter < deviceElements.count(); deviceCounter++)
        {
            if (verbose)
                qDebug()<<"Parsing device element:";
            devices.append(DeviceType(deviceElements.at(deviceCounter).toElement(),vendor,groups,verbose));
        }
    }
}

void sii_tool::write_sii_file(QString siiFilename, int index)
{
    qDebug() << "Writing SII file";

    if (index >= devices.size())
    {
        qDebug()<<"Invalid device index";
        return;
    }

    DeviceType device = devices.at(index);

    QFile siiFile(siiFilename);
    if (!siiFile.open(QIODevice::ReadWrite)) {
        qFatal("Couldn't Open file.");
    }

    if (device.eeprom->definedData) {
        siiFile.write(device.eeprom->siiData); // Binary SII file was defined in ESI file, so just write it to a file.
        siiFile.close();
        return;
    }

    int baseAddr = 0;
    QStringList strings;

    // If we are here, we need to actually assemnbly the sii file, start by making a byte array of the right size
    QByteArray siiData(device.eeprom->size,0);

    // Populate configData section
    siiData.replace(CONFIG_DATA,device.eeprom->configData.length(),device.eeprom->configData);

    // Populate configData checksum
    siiData[CONFIG_CHECK] = computeCRC(device.eeprom->configData,14);

    // Populate general slave information
    setInt32(siiData,VENDOR_ID,device.vendor->vendorID);
    setInt32(siiData,PRODUCT_CODE,device.productCode);
    setInt32(siiData,REV_NUMBER,device.revisionNumber);
    setInt32(siiData,SERIAL_NUMBER,device.serialNumber);

    // Populate bootstrap section
    siiData.replace(BOOTSTRAP_DATA,device.eeprom->bootStrap.length(),device.eeprom->bootStrap);

    // Populate Mailbox configuration
    // TODO: Figure out how to parse the rest of the mailbox values
    setInt16(siiData,MBOX_PROTOCOL,device.mailboxProtocol);

    // Populate footer of slave information section
    setInt16(siiData,EEPROM_SIZE,(device.eeprom->size*8)/1024 - 1); // Size stored in KBit-1 not number of bytes
    setInt16(siiData,VERSION,1);

    // Start category section with strings section
    baseAddr = SII_CATEGORIES;
    int stringsPosAddr = baseAddr+CAT_DATA_OFF;

    assembleStringList(device,strings);
    setInt16(siiData,baseAddr,CAT_TYPE_STRINGS);

    siiData[stringsPosAddr++] = (char)(strings.count());

    for (int stringCount = 0; stringCount < strings.count(); stringCount++)
    {
        siiData[stringsPosAddr++] = strings.at(stringCount).length();
        siiData.replace(stringsPosAddr,strings.at(stringCount).length(),strings.at(stringCount).toAscii());
        stringsPosAddr += strings.at(stringCount).length();
    }

    if (((stringsPosAddr-baseAddr-4) % 2) == 1) // Length is odd, pad one byte
        siiData[stringsPosAddr++] = 0xFF;

    setInt16(siiData,baseAddr+2,(stringsPosAddr-baseAddr-4)/2); // Write length of strings category, divide by two to get word length
    baseAddr = stringsPosAddr; // Update baseAddr for next category

    // Write General Category
    setInt16(siiData,baseAddr,CAT_TYPE_GENERAL);
    setInt16(siiData,baseAddr+CAT_SIZE_OFF,GENERAL_CAT_LENGTH/2); // Divide by two for word length
    baseAddr += CAT_DATA_OFF; // Point baseAddr to beginning of category data.

    if (groups[device.groupId].type.length())
        siiData[baseAddr+GROUP_TYPE_INDEX_OFF] = strings.indexOf(groups[device.groupId].type) + 1;

    // Image data not populated by TwinCAT, so we won't do it either (for now)

    if (device.type.length())
    {
        siiData[baseAddr+ORDER_INDEX_OFF] = strings.indexOf(device.type) + 1;
    }

    if (device.name.length())
    {
        siiData[baseAddr+NAME_INDEX_OFF] = strings.indexOf(device.name) + 1;
    }


    siiData[baseAddr+0x0004] = 1; // For some reason, this reserved byte is set to 1

    siiData[baseAddr+COE_DETAILS_OFF] = (device.enableSDO               ? (1<<0) : 0) |
                                        (device.enableSDOInfo           ? (1<<1) : 0) |
                                        (device.enablePDOAssign         ? (1<<2) : 0) |
                                        (device.enablePDOConfig         ? (1<<3) : 0) |
                                        (device.enableUploadAtStartup   ? (1<<4) : 0) |
                                        (device.enableSDOCompleteAccess ? (1<<5) : 0) ;

    siiData[baseAddr+FOE_DETAILS_OFF] = (device.enableFoE ? 1 : 0);
    siiData[baseAddr+EOE_DETAILS_OFF] = (device.enableEoE ? 1 : 0);
    siiData[baseAddr+FLAGS_OFF] = (device.enableSafeOp       ? (1<<0) : 0) |
                                  (device.enableenableNotLRW ? (1<<1) : 0) ;

    setInt16(siiData,baseAddr+EBUS_CURRENT_OFF,device.ebusCurrent);

    if (groups[device.groupId].name.length())
        siiData[baseAddr+GROUP_NAME_INDEX_OFF] = strings.indexOf(groups[device.groupId].name) + 1;

    setInt16(siiData,baseAddr+PHY_PORT_OFF, ((uint16_t)(device.physics[0]) << 0) |
                                            ((uint16_t)(device.physics[1]) << 4) |
                                            ((uint16_t)(device.physics[2]) << 8) |
                                            ((uint16_t)(device.physics[3]) << 12));

    baseAddr += GENERAL_CAT_LENGTH; // Update base address for next category

    // Populate FMMU category
    setInt16(siiData,baseAddr,CAT_TYPE_FMMU);
    setInt16(siiData,baseAddr+CAT_SIZE_OFF,device.fmmus.length()/2);
    baseAddr += CAT_DATA_OFF;
    for (int count = 0; count < device.fmmus.length(); count++)
        siiData[baseAddr++] = (char)(device.fmmus.at(count).type);

    // Populate Sync Manager category
    setInt16(siiData,baseAddr,CAT_TYPE_SYNCM);
    setInt16(siiData,baseAddr+CAT_SIZE_OFF,device.syncManagers.count()*SYNCM_CAT_LEN/2);
    baseAddr += CAT_DATA_OFF;

    for (int count = 0; count < device.syncManagers.count(); count++)
    {
        setInt16(siiData,baseAddr+SYNCM_PHY_ADDR_OFF,device.syncManagers.at(count).phyStartAddr);
        setInt16(siiData,baseAddr+SYNCM_LENGTH_OFF,device.syncManagers.at(count).length);
        siiData[baseAddr+SYNCM_CTRL_REG_OFF] = device.syncManagers.at(count).controlRegister;

        siiData[baseAddr+SYNCM_ENABLE_OFF] = (device.syncManagers.at(count).enable    ? (1<<0) : 0) |
                                             (device.syncManagers.at(count).virtualSM ? (1<<2) : 0) |
                                             (device.syncManagers.at(count).opOnly    ? (1<<3) : 0) ;

        siiData[baseAddr+SYNCM_TYPE_OFF] = device.syncManagers.at(count).smType;
        baseAddr += SYNCM_CAT_LEN;
    }

    // Populate secondary FMMU category
    //TODO: Figure out what the rest of the bits do
    setInt16(siiData,baseAddr,CAT_TYPE_FMMU_ALT);
    setInt16(siiData,baseAddr+CAT_SIZE_OFF,device.fmmus.count()*2);
    baseAddr += CAT_DATA_OFF;

    for (int count = 0; count < device.fmmus.count(); count++)
    {
        siiData[baseAddr++] = 0b11111000 | (device.fmmus.at(count).suAssigned ? (1<<2) : 0) | (device.fmmus.at(count).smAssigned ? (1<<1) : 0);
        siiData[baseAddr++] = 0xFF;
        siiData[baseAddr++] = (char)device.fmmus.at(count).sm;
        siiData[baseAddr++] = (char)device.fmmus.at(count).su;
    }

    // Populate TX PDO entries
    setInt16(siiData,baseAddr,CAT_TYPE_TXPDO);
    int PDOEntryAddr = baseAddr+CAT_DATA_OFF;

    for (int pdoCount = 0; pdoCount < device.txPDOs.count(); pdoCount++)
    {
        setInt16(siiData,PDOEntryAddr,device.txPDOs[pdoCount].index);
        PDOEntryAddr += 2;
        siiData[PDOEntryAddr++] = device.txPDOs[pdoCount].pdoEntries.count();
        siiData[PDOEntryAddr++] = device.txPDOs[pdoCount].syncManager;
        PDOEntryAddr++; // TODO: Figure out what the Synchronization byte is supposed to be for
        if (device.txPDOs[pdoCount].name.length())
            siiData[PDOEntryAddr++] = strings.indexOf(device.txPDOs[pdoCount].name) + 1;
        setInt16(siiData,PDOEntryAddr, (device.txPDOs[pdoCount].mandatory ? (1<<0) : 0) |
                                       (device.txPDOs[pdoCount].fixed     ? (1<<4) : 0));
        PDOEntryAddr+=2;

        for (int entryCount = 0; entryCount < device.txPDOs[pdoCount].pdoEntries.count(); entryCount++)
        {
            setInt16(siiData,PDOEntryAddr,device.txPDOs[pdoCount].pdoEntries[entryCount].index);
            PDOEntryAddr += 2;
            siiData[PDOEntryAddr++] = device.txPDOs[pdoCount].pdoEntries[entryCount].subIndex;
            if (device.txPDOs[pdoCount].pdoEntries[entryCount].name.length())
                siiData[PDOEntryAddr++] = strings.indexOf(device.txPDOs[pdoCount].pdoEntries[entryCount].name) + 1;
            siiData[PDOEntryAddr++] = device.txPDOs[pdoCount].pdoEntries[entryCount].typeIndex;
            siiData[PDOEntryAddr++] = device.txPDOs[pdoCount].pdoEntries[entryCount].bitLen;
            PDOEntryAddr += 2; // Increment past unused flags;
        }
    }

    setInt16(siiData,baseAddr+CAT_SIZE_OFF,(PDOEntryAddr - CAT_DATA_OFF - baseAddr)/2);
    baseAddr = PDOEntryAddr;

    // Write RX PDO entries
    setInt16(siiData,baseAddr,CAT_TYPE_RXPDO);
    PDOEntryAddr = baseAddr+CAT_DATA_OFF;

    for (int pdoCount = 0; pdoCount < device.rxPDOs.count(); pdoCount++)
    {
        setInt16(siiData,PDOEntryAddr,device.rxPDOs[pdoCount].index);
        PDOEntryAddr += 2;
        siiData[PDOEntryAddr++] = device.rxPDOs[pdoCount].pdoEntries.count();
        siiData[PDOEntryAddr++] = device.rxPDOs[pdoCount].syncManager;
        PDOEntryAddr++; // TODO: Figure out what the Synchronization byte is supposed to be for
        if (device.rxPDOs[pdoCount].name.length())
            siiData[PDOEntryAddr++] = strings.indexOf(device.rxPDOs[pdoCount].name) + 1;
        setInt16(siiData,PDOEntryAddr, (device.rxPDOs[pdoCount].mandatory ? (1<<0) : 0) |
                                       (device.rxPDOs[pdoCount].fixed     ? (1<<4) : 0));
        PDOEntryAddr+=2;

        for (int entryCount = 0; entryCount < device.rxPDOs[pdoCount].pdoEntries.count(); entryCount++)
        {
            setInt16(siiData,PDOEntryAddr,device.rxPDOs[pdoCount].pdoEntries[entryCount].index);
            PDOEntryAddr += 2;
            siiData[PDOEntryAddr++] = device.rxPDOs[pdoCount].pdoEntries[entryCount].subIndex;
            if (device.rxPDOs[pdoCount].pdoEntries[entryCount].name.length())
                siiData[PDOEntryAddr++] = strings.indexOf(device.rxPDOs[pdoCount].pdoEntries[entryCount].name) + 1;
            siiData[PDOEntryAddr++] = device.rxPDOs[pdoCount].pdoEntries[entryCount].typeIndex;
            siiData[PDOEntryAddr++] = device.rxPDOs[pdoCount].pdoEntries[entryCount].bitLen;
            PDOEntryAddr += 2; // Increment past unused flags;
        }
    }

    setInt16(siiData,baseAddr+CAT_SIZE_OFF,(PDOEntryAddr - CAT_DATA_OFF - baseAddr)/2);
    baseAddr = PDOEntryAddr;

    // Write DC Category
    setInt16(siiData,baseAddr,CAT_TYPE_DC);
    setInt16(siiData,baseAddr+CAT_SIZE_OFF,DC_CAT_LENGTH/2);
    baseAddr += CAT_DATA_OFF;

    setInt32(siiData,baseAddr+DC_CYCLE_SYNC0_OFF,device.dcConf->cycleTimeSync0);
    setInt32(siiData,baseAddr+DC_SHIFT_SYNC0_OFF,device.dcConf->shiftTimeSync0);
    setInt32(siiData,baseAddr+DC_SHIFT_SYNC1_OFF,device.dcConf->shiftTimeSync1);
    setInt16(siiData,baseAddr+DC_CYCLE_SYNC1_FACTOR_OFF,device.dcConf->cycleTimeSync1Factor);
    setInt16(siiData,baseAddr+DC_ASSIGN_ACTIVATE_OFF,device.dcConf->assignActivateWord);
    setInt16(siiData,baseAddr+DC_CYCLE_SYNC0_FACTOR_OFF,device.dcConf->cycleTimeSync0Factor);
    if (device.dcConf->DCSyncName.length())
        siiData[baseAddr+DC_SYNC_NAME_INDEX_OFF] = strings.indexOf(device.dcConf->DCSyncName) + 1;
    baseAddr += DC_CAT_LENGTH;

    // Fill rest of eeprom with 0xFF
    for(int count = baseAddr; count < siiData.length(); count++)
        siiData[count] = 0xFF;

    siiFile.write(siiData);
    siiFile.close();
}

char sii_tool::computeCRC(QByteArray data, int length)
{
    // This function implements a CRC8 with initial value of 0xFF

    char current_char;
    char bit;
    char crc = 0xFF;
    for (int byte = 0; byte < length; byte++)
    {
        if (byte < data.length())
            current_char = data.at(byte);
        else
            current_char = 0;

        for (unsigned long bitPos = 0x80; bitPos; bitPos>>=1)
        {
            bit = crc & (1<<7);
            crc<<= 1;
            if (current_char & bitPos)
                bit^= (1<<7);
            if (bit)
                crc^= 0x07;
        }
    }

    return(crc);
}

void sii_tool::setInt16(QByteArray &data, int address, uint16_t value)
{
    data[address] = value & 0xFF;
    data[address+1] = (value>>8) & 0xFF;
}

void sii_tool::setInt32(QByteArray &data, int address, uint32_t value)
{
    data[address] = value & 0xFF;
    data[address+1] = (value>>8) & 0xFF;
    data[address+2] = (value>>16) & 0xFF;
    data[address+3] = (value>>24) & 0xFF;
}

void sii_tool::assembleStringList(DeviceType &device, QStringList &strings)
{
    //TODO: Add parsing of image data
    // Generate list of device strings with same order as TwinCAT creates
    if (device.type.length())
        strings.append(device.type);

    if(groups[device.groupId].type.length() && (strings.indexOf(groups[device.groupId].type) == -1))
        strings.append(groups[device.groupId].type);

    if(groups[device.groupId].name.length() && (strings.indexOf(groups[device.groupId].name) == -1))
        strings.append(groups[device.groupId].name);

    if (device.name.length() && (strings.indexOf(device.name) == -1))
        strings.append(device.name);

    if (device.dcConf->DCSyncName.length() && (strings.indexOf(device.dcConf->DCSyncName) == -1))
        strings.append(device.dcConf->DCSyncName);

    for (int PDOcount = 0; PDOcount < device.txPDOs.count(); PDOcount++)
    {
        if (device.txPDOs.at(PDOcount).name.length() && (strings.indexOf(device.txPDOs.at(PDOcount).name) == -1))
            strings.append(device.txPDOs.at(PDOcount).name);
        for (int entryCount = 0; entryCount < device.txPDOs.at(PDOcount).pdoEntries.count(); entryCount++)
        {
            if (device.txPDOs.at(PDOcount).pdoEntries.at(entryCount).name.length() &&
                    (strings.indexOf(device.txPDOs.at(PDOcount).pdoEntries.at(entryCount).name) == -1))
                strings.append(device.txPDOs.at(PDOcount).pdoEntries.at(entryCount).name);
        }
    }

    for (int PDOcount = 0; PDOcount < device.rxPDOs.count(); PDOcount++)
    {
        if (device.rxPDOs.at(PDOcount).name.length() && (strings.indexOf(device.rxPDOs.at(PDOcount).name) == -1))
            strings.append(device.rxPDOs.at(PDOcount).name);
        for (int entryCount = 0; entryCount < device.rxPDOs.at(PDOcount).pdoEntries.count(); entryCount++)
        {

            if (device.rxPDOs.at(PDOcount).pdoEntries.at(entryCount).name.length() &&
                    (strings.indexOf(device.rxPDOs.at(PDOcount).pdoEntries.at(entryCount).name) == -1))
                strings.append(device.rxPDOs.at(PDOcount).pdoEntries.at(entryCount).name);
        }
    }

}
