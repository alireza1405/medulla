#ifndef SII_TOOL_H
#define SII_TOOL_H

#include <QWidget>
#include <QDebug>
#include <eepromtype.h>
#include <vendortype.h>
#include <entrytype.h>
#include <pdotype.h>

namespace Ui {
class sii_tool;
}

class sii_tool : public QWidget
{
    Q_OBJECT
    
public:
    explicit sii_tool(QWidget *parent = 0);
    ~sii_tool();
    
private:
    Ui::sii_tool *ui;
};

#endif // SII_TOOL_H
