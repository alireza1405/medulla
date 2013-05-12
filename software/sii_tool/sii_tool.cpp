#include "sii_tool.h"
#include "ui_sii_tool.h"

sii_tool::sii_tool(QStringList args, QObject *parent) :
    QObject(parent)
{
    vendor = 0;
    if (args.indexOf("-v") >= 0)
        verbose = true;
    else
        verbose = false;

    parse_esi_file(args.at(1));
    write_sii_file("/home/kit/medulla/software/sii_tool/test.bin",0);
    exit(0);
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
        qDebug()<<"Parsing vendor element";
    vendor = new VendorType(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Vendor").at(0).toElement(),verbose);

    if (verbose)
        qDebug()<<"Parsing device elements";
    QDomNodeList deviceElements = doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Device");
    for (int count = 0; count < deviceElements.count(); count++)
        devices.append(DeviceType(deviceElements.at(count).toElement(),vendor,verbose));
}

void sii_tool::write_sii_file(QString siiFilename, int index)
{
    qDebug() << "Writing SII file";

    if (index >= devices.size())
    {
        qDebug()<<"Invalid device index";
        return;
    }

    DeviceType device = devices.at(0);

    QFile siiFile(siiFilename);
    if (!siiFile.open(QIODevice::ReadWrite)) {
        qFatal("Couldn't Open file.");
    }

    if (device.eeprom->definedData) {
        siiFile.write(device.eeprom->siiData); // Binary SII file was defined in ESI file, so just write it to a file.
        siiFile.close();
        return;
    }

    // If we are here, we need to actually assemnbly the sii file, start by making a byte array of the right size
    QByteArray siiData(device.eeprom->size,0);

    // Populate configData section
    siiData.replace(CONFIG_DATA,14,device.eeprom->configData.data(),14);

    // Populate configData checksum
    siiData[CONFIG_CHECK] = computeCRC(device.eeprom->configData,14);

    // Populate general slave information
    setInt32(siiData,VENDOR_ID,device.vendor->vendorID);
    setInt32(siiData,PRODUCT_CODE,device.productCode);
    setInt32(siiData,REV_NUMBER,device.revisionNumber);
    setInt32(siiData,SERIAL_NUMBER,device.serialNumber);

    // Populate bootstrap section
    siiData.replace(BOOTSTRAP_DATA,8,device.eeprom->bootStrap.data(),8);

    // Populate Mailbox configuration
    // TODO: Figure out how to parse the rest of the mailbox values
    setInt16(siiData,MBOX_PROTOCOL,device.mailboxProtocol);

    // Populate footer of slave information section
    setInt16(siiData,EEPROM_SIZE,(device.eeprom->size*8)/1024 - 1); // Size stored in KBit-1 not number of bytes
    setInt16(siiData,VERSION,1);

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
            //qDebug("%x\n",bitPos);
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

void sii_tool::setInt16(QByteArray &data, int address, qint16 value)
{
    data[address] = value & 0xFF;
    data[address+1] = (value>>8) & 0xFF;
}

void sii_tool::setInt32(QByteArray &data, int address, qint32 value)
{
    data[address] = value & 0xFF;
    data[address+1] = (value>>8) & 0xFF;
    data[address+2] = (value>>16) & 0xFF;
    data[address+3] = (value>>24) & 0xFF;
}
