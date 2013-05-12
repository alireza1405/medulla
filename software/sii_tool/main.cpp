#include <QCoreApplication>
#include "sii_tool.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    sii_tool w(a.arguments());
    //w.show();
    
    return a.exec();
}
