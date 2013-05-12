#include "entrytype.h"

EntryType::EntryType(QDomElement element, bool verbose)
{
    QDomNodeList nodes;
    verb = verbose;

    // Parse Name element if there is one
    nodes = element.elementsByTagName("Name");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Name element:"<<nodes.at(0).toElement().text();;
        name = nodes.at(0).toElement().text();
    }

    // Parse Index element if there is one
    nodes = element.elementsByTagName("Index");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Index element:"<<nodes.at(0).toElement().text();
        QString indexStr = nodes.at(0).toElement().text();
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((indexStr.at(0) == '#') && (indexStr.at(1) == 'x'))
            index = indexStr.remove(0,2).toInt(0,16);
        else
            index = indexStr.toInt(0,0);
    }
    else
        qFatal("No index found");

    // Parse SubIndex element if there is one
    nodes = element.elementsByTagName("SubIndex");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found SubIndex element:"<<nodes.at(0).toElement().text();
        QString subIndexStr = nodes.at(0).toElement().text();
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((subIndexStr.at(0) == '#') && (subIndexStr.at(1) == 'x'))
            subIndex = subIndexStr.remove(0,2).toInt(0,16);
        else
            subIndex = subIndexStr.toInt(0,0);
    }
    else
        qFatal("No sub indx found");

    // Parse BitLen element if there is one
    nodes = element.elementsByTagName("BitLen");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found BitLen element:"<<nodes.at(0).toElement().text();
        QString bitLenStr = nodes.at(0).toElement().text();
        // since it would appear the default hex encoding is #x0000 then we need to catch this our selves
        if ((bitLenStr.at(0) == '#') && (bitLenStr.at(1) == 'x'))
            bitLen = bitLenStr.remove(0,2).toInt(0,16);
        else
            bitLen = bitLenStr.toInt(0,0);
    }

    // Parse DataType element if there is one
    nodes = element.elementsByTagName("DataType");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found DataType element:"<<nodes.at(0).toElement().text();
        QString dataTypeStr = nodes.at(0).toElement().text();

        if ((dataTypeStr.compare("BOOL") == 0) || (dataTypeStr.compare("BIT") == 0))
        {
            typeIndex = 0x0001;
            bitLen = 1;
        }
        else if (dataTypeStr.compare("USINT") == 0)
        {
            typeIndex = 0x0008;
            bitLen = 8;
        }
        else if (dataTypeStr.compare("BIT1") == 0)
        {
            typeIndex = 0x0030;
            bitLen = 1;
        }
        else if (dataTypeStr.compare("BIT2") == 0)
        {
            typeIndex = 0x0031;
            bitLen = 2;
        }
        else if (dataTypeStr.compare("BIT3") == 0)
        {
            typeIndex = 0x0032;
            bitLen = 3;
        }
        else if (dataTypeStr.compare("BIT4") == 0)
        {
            typeIndex = 0x0033;
            bitLen = 4;
        }
        else if (dataTypeStr.compare("BIT5") == 0)
        {
            typeIndex = 0x0034;
            bitLen = 5;
        }
        else if (dataTypeStr.compare("BIT6") == 0)
        {
            typeIndex = 0x0035;
            bitLen = 6;
        }
        else if (dataTypeStr.compare("BIT7") == 0)
        {
            typeIndex = 0x0036;
            bitLen = 7;
        }
        else if (dataTypeStr.compare("BIT8") == 0)
        {
            typeIndex = 0x0037;
            bitLen = 8;
        }
        else if (dataTypeStr.compare("BITARR8") == 0)
        {
            typeIndex = 0x002D;
            bitLen = 8;
        }
        else if (dataTypeStr.compare("BITARR16") == 0)
        {
            typeIndex = 0x002E;
            bitLen = 16;
        }
        else if (dataTypeStr.compare("BITARR32") == 0)
        {
            typeIndex = 0x002F;
            bitLen = 32;
        }
        else if (dataTypeStr.compare("SINT") == 0)
        {
            typeIndex = 0x0002;
            bitLen = 8;
        }
        else if (dataTypeStr.compare("INT") == 0)
        {
            typeIndex = 0x0003;
            bitLen = 16;
        }
        else if (dataTypeStr.compare("INT24") == 0)
        {
            typeIndex = 0x0010;
            bitLen = 24;
        }
        else if (dataTypeStr.compare("DINT") == 0)
        {
            typeIndex = 0x0004;
            bitLen = 32;
        }
        else if (dataTypeStr.compare("INT40") == 0)
        {
            typeIndex = 0x0012;
            bitLen = 40;
        }
        else if (dataTypeStr.compare("INT48") == 0)
        {
            typeIndex = 0x0013;
            bitLen = 48;
        }
        else if (dataTypeStr.compare("INT56") == 0)
        {
            typeIndex = 0x0014;
            bitLen = 56;
        }
        else if (dataTypeStr.compare("LINT") == 0)
        {
            typeIndex = 0x0015;
            bitLen = 64;
        }
        else if (dataTypeStr.compare("USINT") == 0)
        {
            typeIndex = 0x0005;
            bitLen = 8;
        }
        else if (dataTypeStr.compare("UINT") == 0)
        {
            typeIndex = 0x0006;
            bitLen = 16;
        }
        else if (dataTypeStr.compare("UINT24") == 0)
        {
            typeIndex = 0x00016;
            bitLen = 24;
        }
        else if (dataTypeStr.compare("UDINT") == 0)
        {
            typeIndex = 0x0007;
            bitLen = 32;
        }
        else if (dataTypeStr.compare("UINT40") == 0)
        {
            typeIndex = 0x0018;
            bitLen = 40;
        }
        else if (dataTypeStr.compare("UINT48") == 0)
        {
            typeIndex = 0x0019;
            bitLen = 48;
        }
        else if (dataTypeStr.compare("UINT56") == 0)
        {
            typeIndex = 0x001A;
            bitLen = 56;
        }
        else if (dataTypeStr.compare("ULINT") == 0)
        {
            typeIndex = 0x001B;
            bitLen = 64;
        }
        else if (dataTypeStr.compare("REAL") == 0)
        {
            typeIndex = 0x0008;
            bitLen = 32;
        }
        else if (dataTypeStr.compare("LREAL") == 0)
        {
            typeIndex = 0x00011;
            bitLen = 64;
        }
        else
            qFatal("Unknown data type");
    }
}
