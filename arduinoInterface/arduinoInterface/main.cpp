#include <QCoreApplication>
#include <QSettings>

#include "arduinointerface.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Seraph");
    QCoreApplication::setOrganizationDomain("seraphrobotics.com");
    QCoreApplication::setApplicationName("arduinoInterface");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    ArduinoInterface* ai = new ArduinoInterface();
    ai->connectPort("COM5",BAUD9600);
    QString s = "G1 X10 Y10 Z10 F99";
    QStringList sl;
    sl.append(s);
    ai->writeCommands(sl);

    return a.exec();
}
