#include "eepromtype.h"

eepromType::eepromType(QDomElement element, bool verbose)
{
    QDomNodeList nodes; // Used to hold returns from elementsByTagName
    verb = verbose;

    // Parse ByteSize element if there is one
    nodes = element.elementsByTagName("ByteSize");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found ByteSize element:"<<nodes.at(0).toElement().text();;
        size = nodes.at(0).toElement().text().toInt();
    }

    // Parse ConfigData element if there is one
    nodes = element.elementsByTagName("ConfigData");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<< "Found ConfigData Element:"<<nodes.at(0).toElement().text();
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        configData.clear();

        for (int i = 0; i < chars.size(); i+=2)
        {
            if ((i/2) > 14) // The config data section was too long, just stop now
                break;

            char inchar;
            inchar  = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55))<<4;
            inchar |= (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55));
            configData.append(inchar);
        }
    }

    // Parse BootStrap element if there is one
    nodes = element.elementsByTagName("BootStrap");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found BootStrap element:"<<nodes.at(0).toElement().text();;
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        bootStrap.clear();

        for (int i = 0; i < chars.size(); i+=2)
        {
            if ((i/2) > 8) // The bootstrap section was too long, just stop now
                break;
            char inchar;
            inchar  = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55))<<4;
            inchar |= (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55));
            bootStrap.append(inchar);
        }
    }

    // Check for a data element, if there is one, then knock out the whole data vector and replace it
    nodes = element.elementsByTagName("Data");
    if (nodes.count() > 0)
    {
        if (verb)
            qDebug()<<"Found Data element:"<<nodes.at(0).toElement().text();;
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        siiData.clear();

        for (int i = 0; i < chars.size(); i+=4)
        {
            char inchar;
            inchar  = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55))<<4;
            inchar |= (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55));
            eepromData.append(inchar);
        }

        definedData = true;
    }

}
