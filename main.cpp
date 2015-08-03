#include <QApplication>
#include "mainwindow.h"
//#include "Interface.h"
#include <stdio.h>
#include <iostream>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QSettings setup. These values will carry over in the rest of the application.
    QCoreApplication::setOrganizationName("Seraph");
    QCoreApplication::setOrganizationDomain("seraphrobotics.com");
    QCoreApplication::setApplicationName("WellPlater");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    // Default scope is QSettings::UserScope

    // This is the list of values the application is storing:
    //   config_dir
    //   load config next time index
    //   last used fab file

    QSettings s;
    s.setValue("home-url","http://mark-broomfield.com/seraph/#/login.html");
    s.sync();
    MainWindow w;

    w.show();

    return a.exec();
}
