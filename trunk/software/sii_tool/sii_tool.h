#ifndef SII_TOOL_H
#define SII_TOOL_H

//#include <QWidget>
#include <QDebug>
#include <eepromtype.h>
#include <vendortype.h>
#include <entrytype.h>
#include <pdotype.h>
#include <devicetype.h>
#include <QList>

class sii_tool : public QObject
{
    Q_OBJECT
    
public:
    explicit sii_tool(QObject *parent = 0);
    ~sii_tool();
    
private:
    QList<DeviceType> devices;
    VendorType *vendor;  // Assuming there will only ever be one vendor per esi file.

    void parse_esi_file(QString filename);
    void write_sii_file(QString sii_file);
};

#endif // SII_TOOL_H
