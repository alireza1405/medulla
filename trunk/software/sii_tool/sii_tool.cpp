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
    if (index >= devices.size())
    {
        qDebug()<<"Invalid device index";
        return;
    }

    DeviceType device = devices.at(0);

    QFile siiFile(siiFilename);
    if (!siiFile.open(QIODevice::WriteOnly)) {
        qFatal("Couldn't Open file.");
    }

    if (device.eeprom->definedData) {
        siiFile.write(device.eeprom->siiData); // Binary SII file was defined in ESI file, so just write it to a file.
        siiFile.flush();
        siiFile.close();
        return;
    }

    // If we are here, we need to actually assemnbly the sii file, start by making a byte array of the right size
    QByteArray siiData(device.eeprom->size,0);
}
