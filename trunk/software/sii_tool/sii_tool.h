#ifndef SII_TOOL_H
#define SII_TOOL_H

#include <QDebug>
#include <eepromtype.h>
#include <vendortype.h>
#include <entrytype.h>
#include <pdotype.h>
#include <devicetype.h>
#include <QList>
#include <QStringList>
#include <sii_file.h>
#include <grouptype.h>

class sii_tool : public QObject
{
    Q_OBJECT
    
public:
    explicit sii_tool(QStringList args, QObject *parent = 0);
    ~sii_tool();
    
private:
    QList<DeviceType> devices;
    QList<GroupType> groups;
    VendorType *vendor;  // Assuming there will only ever be one vendor per esi file.
    bool verbose;

    void parse_esi_file(QString filename);
    void write_sii_file(QString siiFilename, int index);
    char computeCRC(QByteArray data, int length);
    void setInt16(QByteArray &data, int address, uint16_t value);
    void setInt32(QByteArray &data, int address, uint16_t value);
    QStringList assembleStringList(DeviceType device);
};

#endif // SII_TOOL_H
