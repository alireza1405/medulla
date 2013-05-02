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

    qDebug()<<"Creating device Object";
    pdoType pdo(doc.elementsByTagName("EtherCATInfo").at(0).toElement().elementsByTagName("Device").at(0).toElement());
}

sii_tool::~sii_tool()
{
    delete ui;
}
