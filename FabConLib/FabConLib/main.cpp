#include "virtualmachines.h"
#include <stdio.h>
#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

//#include "nmotion.h"
//#include "npath.h"
#include "testing/util.h"
#include "testing/unittests.h"

#include "coreinterface.h"
//#include "testing/testerforci.h"
//#include "testing/jscitest.h"



int main(int argc, char *argv[]) {
    printf("\nStarting..");
    QCoreApplication app(argc,argv);
//    TestPrinter vm;
////    VirtualPrinter vm;

////    //load the config file into the DOM document
//    QString configFilePath="../../SeraphPrint/media/SeraphScientistSingleDisplacement.config";
//    QDomDocument document;
//    {
//      QFile configFile(configFilePath);
//      if (!configFile.open(QFile::ReadOnly)) {
//          printf("\nFAILED TO OPEN CONFIG FILE\n");
//          return app.exec();
//      }
//      document.setContent(&configFile);
//      configFile.close();
//    }
//    vm.loadConfig(document);// Load the config into the VM to create the electronics
//    if (vm.isInitialized()){
//        printf("\nVM initialized");
//    }
////        //testNPaths(&vm);
////        //testScripting(&vm);
////        //testVoxels(&vm);
////        //testXDFLPaths(&vm);
//        testXDFLParsing(&vm);
//        QTimer::singleShot(10000,&vm,SLOT(dumpstates()));
////        vm.dumpstates();
////        printState("\n CurPos:",vm.currentState());
////        printf("\nErrors: %s",vm.getErrors().toStdString().c_str());
//        printf("\nDone\n");


//    testerforci *tester = new testerforci();
//    QTimer::singleShot(0,tester,SLOT(setConfig()));

//    JsCiTest *jstester = new JsCiTest();
//    QTimer::singleShot(0,jstester,SLOT(test()));

    ArduinoInterface* ai = new ArduinoInterface("COM3",BAUD115200);
    testNewAIWriteTest(ai);


    return app.exec();//return 0;
}
