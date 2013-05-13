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

class sii_tool
{
    
public:
    explicit sii_tool(bool verb);
    void parse_esi_file(QString filename);
    void write_sii_file(QString siiFilename, int index);
    QList<DeviceType> devices;
    ~sii_tool();
    
private:

    QList<GroupType> groups;
    VendorType *vendor;  // Assuming there will only ever be one vendor per esi file.
    bool verbose;


    char computeCRC(QByteArray data, int length);
    void setInt16(QByteArray &data, int address, uint16_t value);
    void setInt32(QByteArray &data, int address, uint32_t value);
    void assembleStringList(DeviceType &device, QStringList &strings);
};

#endif // SII_TOOL_H
