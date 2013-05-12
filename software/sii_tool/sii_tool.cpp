#include "sii_tool.h"
#include "ui_sii_tool.h"

sii_tool::sii_tool(QObject *parent)
{
    vendor = 0;
    parse_esi_file("/home/kit/medulla/software/sii_tool/testESI.xml");
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
    vendor = new VendorType(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Vendor").at(0).toElement());

    qDebug()<<"Parsing device elements";
    QDomNodeList deviceElements = doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Device");
    for (int count = 0; count < deviceElements.count(); count++)
        devices.append(DeviceType(deviceElements.at(count).toElement(),vendor));
}

void sii_tool::write_sii_file(QString sii_file)
{

}
