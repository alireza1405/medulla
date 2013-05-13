#include <QCoreApplication>
#include "sii_tool.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    bool verbose;
    QStringList args = a.arguments();

    if ((args.indexOf("--help") != -1) || (args.count() == 1))
    {
        qDebug()<<"Usage: sii_tool [OPTION]... [ESI FILE]";
        qDebug()<<"Generates EtherCAT SII file from a given xml ESI file.";
        qDebug()<<"";
        qDebug()<<"Options:";
        qDebug()<<"  -a            Generate SII files for all devices in ESI file";
        qDebug()<<"  -i <index>    Generate SII file for a given zero indexed device in the ESI file";
        qDebug()<<"  -o <file>     Places SII file in gievn file, only works with -i";
        return 0;
    }

    QString pwd("");
    char * PWD;
    PWD = getenv ("PWD");
    pwd.append(PWD);
    pwd.append("/");

    if (args.indexOf("-v") >= 0)
        verbose = true;
    else
        verbose = false;

    int index = -1;
    if (args.indexOf("-i") != -1)
    {
        if (args.indexOf("-i") < args.length()-1)
        {
            if (!args[args.indexOf("-i")+1].startsWith("-"))
                index = args.at(args.indexOf("-i")+1).toInt();
            else
            {
                qDebug()<<"Bad Index";
                return 0;
            }
        }
        else
        {
            qDebug()<<"Bad index";
            return 0;
        }
    }

    bool all;
    if (args.indexOf("-a") != -1)
        all = true;
    else
        all = false;

    QString outFile;
    if (args.indexOf("-o") != -1)
    {
        if (args.indexOf("-o") < args.length()-1)
        {
            if (!args[args.indexOf("-o")+1].startsWith("-"))
            {
                outFile = args.at(args.indexOf("-o")+1);
            }
            else
            {
                qDebug()<<"Missing output file";
                return 0;
            }
        }
        else
        {
            qDebug()<<"Missing output file";
            return 0;
        }
    }

    sii_tool w(verbose);

    if (args.length() == 1)
    {
        qDebug()<<"No Input File given";
        return 0;
    }

    w.parse_esi_file(args.last());


    if (all)
    {
        for (int count = 0; count < w.devices.count(); count++)
        {
            QString filename = pwd;
            filename.append(w.devices[count].type.replace(" ","_"));
            filename.append(".bin");
            w.write_sii_file(filename,count);
        }
    }
    else if (index != -1) {
        if (outFile.isEmpty())
        {
            QString filename = pwd;
            filename.append(w.devices[index].type.replace(" ","_"));
            filename.append(".bin");
            w.write_sii_file(filename,index);
        }
        else
            w.write_sii_file(outFile,index);

    }

    return 0;
}
