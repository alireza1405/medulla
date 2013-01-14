#include "eepromtype.h"

eepromType::eepromType(QDomElement element)
{
    data.resize(0x0040); // Set the data section to the minimum size allowed by the ethercat spec.
    QDomNodeList nodes; // Used to hold returns from elementsByTagName

    // Parse ByteSize element if there is one
    nodes = element.elementsByTagName("ByteSize");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found ByteSize element:"<<nodes.at(0).toElement().text();;
        setSize(nodes.at(0).toElement().text().toInt());
    }

    // Parse ConfigData element if there is one
    nodes = element.elementsByTagName("ConfigData");
    if (nodes.count() > 0)
    {
        qDebug()<< "Found ConfigData Element:"<<nodes.at(0).toElement().text();
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        unsigned short configDataWords[7];
        for (int i = 0; i < chars.size(); i+=4)
        {
            if ((i/4) > 7) // The config data section was too long, just stop now
                break;

            configDataWords[i/4] = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55))<<4;
            configDataWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55));
            configDataWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+2]) <= 57) ? (chars[i+2]-48) : (chars[i+2] - 55)) << 12;
            configDataWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+3]) <= 57) ? (chars[i+3]-48) : (chars[i+3] - 55)) << 8;
        }
        setConfigData(configDataWords);
    }

    // Parse BootStrap element if there is one
    nodes = element.elementsByTagName("BootStrap");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found BootStrap element:"<<nodes.at(0).toElement().text();;
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        unsigned short bootStrapWords[4];
        for (int i = 0; i < chars.size(); i+=4)
        {
            if ((i/4) > 4) // The bootstrap section was too long, just stop now
                break;

            bootStrapWords[i/4] = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55))<<4;
            bootStrapWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55));
            bootStrapWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+2]) <= 57) ? (chars[i+2]-48) : (chars[i+2] - 55)) << 12;
            bootStrapWords[i/4] |= (uint16_t)(((uint8_t)(chars[i+3]) <= 57) ? (chars[i+3]-48) : (chars[i+3] - 55)) << 8;
        }
        setBootStrap(bootStrapWords);
    }

    // Check for a data element, if there is one, then knock out the whole data vector and replace it
    nodes = element.elementsByTagName("Data");
    if (nodes.count() > 0)
    {
        qDebug()<<"Found Data element:"<<nodes.at(0).toElement().text();;
        QByteArray chars = nodes.at(0).toElement().text().toAscii();
        data.clear();
        uint16_t tempWord;
        for (int i = 0; i < chars.size(); i+=4)
        {
            tempWord = (uint16_t)(((uint8_t)(chars[i]) <= 57) ? (chars[i]-48) : (chars[i] - 55));
            if ((i+1) < chars.size())
                tempWord = (uint16_t)(((uint8_t)(chars[i+1]) <= 57) ? (chars[i+1]-48) : (chars[i+1] - 55)) << 8;

            if ((i+2) < chars.size())
                tempWord = (uint16_t)(((uint8_t)(chars[i+2]) <= 57) ? (chars[i+2]-48) : (chars[i+2] - 55)) << 16;

            if ((i+3) < chars.size())
                tempWord = (uint16_t)(((uint8_t)(chars[i+3]) <= 57) ? (chars[i+3]-48) : (chars[i+3] - 55)) << 24;

            data.push_back(tempWord);
        }

        definedData = true;
    }

}

void eepromType::setSize(uint16_t newSize)
{
    size = newSize;
    data.resize(newSize/2); // Resize the data vector
    data[0x003E] = ((newSize*8)/1024)-1; // Write the eeprom size into the data vector
}

void eepromType::setConfigData(uint16_t newConfigData[7])
{
    for (int word = 0; word < 7; word++)
    {
        configData[word] = newConfigData[word];
        data[word] = newConfigData[word];
    }

    // Compute checksum
    //TODO: Compute Checksum
    data[0x0007] = 0x00F2;
}

void eepromType::setBootStrap(uint16_t newBootStrap[4])
{
    for (int word = 0; word < 4; word++)
    {
        bootStrap[word] = newBootStrap[word];
        data[word+0014] = newBootStrap[word];
    }
}

uint16_t eepromType::getSize()
{
    return size;
}

uint16_t* eepromType::getConfigData()
{
    return configData;
}

uint16_t* eepromType::getBootStrap()
{
    return bootStrap;
}

bool eepromType::hasDefinedData()
{
    return definedData;
}
