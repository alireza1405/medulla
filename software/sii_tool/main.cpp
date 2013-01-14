#include <QApplication>
#include "sii_tool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sii_tool w;
    w.show();
    
    return a.exec();
}
