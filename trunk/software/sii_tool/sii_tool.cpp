#include "sii_tool.h"
#include "ui_sii_tool.h"

sii_tool::sii_tool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sii_tool)
{
    ui->setupUi(this);
    QDomDocument doc("ESIFile");
    QFile file("testESI.xml");

    qDebug()<< "Opening File";
    if (!file.open(QIODevice::ReadOnly))
        return;

    qDebug()<<"Loading File";
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    /*qDebug()<<"Creating eepromType object";
    eepromType eeprom(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Eeprom").at(0).toElement());

    qDebug()<<"Creating vendorType object";
    VendorType vendor(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Vendor").at(0).toElement());

    qDebug()<<"creating entryType object";
    EntryType entry(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Entry").at(0).toElement());
    qDebug()<<"Index:"<<entry.index;
    qDebug()<<"SubIndex:"<<entry.subIndex;
    qDebug()<<"BitLen:"<<entry.bitLen;
    qDebug()<<"Name:"<<entry.name;
    qDebug()<<"DataTypeIndex"<<entry.typeIndex;*/

    qDebug()<<"Creating PDO Object";
    pdoType pdo(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("TxPdo").at(0).toElement());
    qDebug()<<"Sm:"<<pdo.syncManager;
    qDebug()<<"Fixed:"<<pdo.fixed;
    qDebug()<<"Mand:"<<pdo.mandatory;
}

sii_tool::~sii_tool()
{
    delete ui;
}
