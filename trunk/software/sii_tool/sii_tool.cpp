#include "sii_tool.h"
#include "ui_sii_tool.h"

sii_tool::sii_tool(QStringList args, QObject *parent)
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

    qDebug()<< "Opening File";
    if (!file.open(QIODevice::ReadOnly))
        return;

    qDebug()<<"Loading File";
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    qDebug()<<"Parsing vendor element";
    vendor = new VendorType(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Vendor").at(0).toElement(),verbose);

    qDebug()<<"Parsing device elements";
    QDomNodeList deviceElements = doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Device");
    for (int count = 0; count < deviceElements.count(); count++)
        devices.append(DeviceType(deviceElements.at(count).toElement(),vendor,verbose));
}

void sii_tool::write_sii_file(QString sii_file)
{

}
