#include "virtualmachines.h"
#include <stdio.h>
#include <QTextStream>
#include <QFile>
//#include "jsnpath.h"

#include "testing/util.h"
#include "xdflvoxel.h"
#include <QFile>

VMPrototype::VMPrototype():initialized_(false),printing(false){
    warmup=QStringList();
    cooldown=QStringList();
}

bool VMPrototype::isInitialized() {
    return initialized_;
}



void VMPrototype::setComPort(QString comPort) {
    if (!comPort.isNull()) {comPort_=comPort;}
}

QString VMPrototype::getErrors() {
    return error_string_;
}


void VMPrototype::startprint(){
    printing = true;
}
void VMPrototype::cancelprint(){
    printing = false;
}
void VMPrototype::pauseprint(){
    printing = false;
}

int VMPrototype::getBufferSize(){return 0;}

QVector<double> VMPrototype::currentPosition(){
    return QVector<double>(3,0.0);
}

void VMPrototype::runCmds(QStringList sl){}

bool VMPrototype::moveTo(double x, double y, double z, double speed) {

    return false;
}

bool VMPrototype::move(double x, double y, double z, double speed) {

    return false;
}

bool VMPrototype::forceStop(){
    return true;
}

void VMPrototype::resetPosition(){
    return;
}

void VMPrototype::dumpstates(){}

QScriptEngine* VMPrototype::makeEngine(){


    QScriptEngine* engine = new QScriptEngine();


//    // State object
//    int statesize = nstatesize();
//    engine->globalObject().setProperty("statesize",statesize);
//    QScriptValue stateProto = engine->newArray(statesize);
//    for(int j=0;j<statesize;j++) {
//        stateProto.setProperty(j,0);
//    }
//    stateProto.setProperty("distance", 0);
//    engine->globalObject().setProperty("State", engine->newFunction(newJsState,stateProto));

//    QScriptValue npathProto = engine->newArray(0);
//    engine->globalObject().setProperty("NPath",engine->newFunction(newJsNPath,npathProto));

    return engine;
}


void VMPrototype::loadConfig(QDomDocument document) {
    QDomElement root = document.documentElement();





    // XYZMOTION
    QDomNode motion = root.namedItem("motion");
    xyzmotion = new XYZMotion(motion);


    // BAYS
    QDomNode tools = root.namedItem("tool");
    QDomNodeList toolChildren  = tools.childNodes();
    bays.clear();
    bays = QList<Bay*>();
    for (unsigned int k = 0; k < toolChildren.length(); k++) {
        if ("bay"==toolChildren.at(k).nodeName().toLower()) {
            bays.append(new Bay(toolChildren.at(k)));
            bays.last()->setEngine(makeEngine());
        }
    }

    QDomNode system = root.namedItem("system");
    QDomNodeList systemChildren  = system.childNodes();
    for (unsigned int k = 0; k < systemChildren.length(); k++) {
        if ("warmup"==systemChildren.at(k).nodeName().toLower()) {
            warmup = systemChildren.at(k).toElement().text().split(";");
        }else if ("cooldown"==systemChildren.at(k).nodeName().toLower()){
            cooldown = systemChildren.at(k).toElement().text().split(";");
        }
    }



    initialized_ = true;
}





///////////////////////////////////////////////////////////////////////////////////////////

VirtualPrinter::VirtualPrinter():VMPrototype(),buffsize(0) {
    ai_ = new ArduinoInterface();
    ai_->moveToThread(&workerThread);
    workerThread.start();
    connect(ai_,SIGNAL(queuesize(int)),this,SLOT(setbuffsize(int)));
}

VirtualPrinter::~VirtualPrinter(){
    workerThread.quit();
    workerThread.wait();
}

void VirtualPrinter::loadConfig(QDomDocument document) {

    QDomElement root = document.documentElement();
//    QDomNode electronics = root.namedItem("electronics");

//    delete ai_;
//    ai_ = new ArduinoInterface(comPort_,BAUD115200);
//    ai_->start();
    ai_->connectPort(comPort_,BAUD115200);
    connect(ai_,SIGNAL(queuesize(int)),this,SLOT(setbuffsize(int)));
    if (ai_->isReady()){
        initialized_ = true;
    }
    // ELECTRONICS INTERFACE
//    eInterface.initialize(electronics,comPort_);


    // XYZMOTION
    QDomNode motion = root.namedItem("motion");
    xyzmotion =new XYZMotion(motion);

    //BAYS
    QDomNode tools = root.namedItem("tool");
    QDomNodeList toolChildren  = tools.childNodes();

    bays.clear();
    bays = QList<Bay*>();

    for(uint k=0; k<toolChildren.length();k++){
        if ("bay"==toolChildren.at(k).nodeName().toLower()){
            bays.append(new Bay(toolChildren.at(k)));
            bays.last()->setEngine(makeEngine());
        }
    }

    QDomNode system = root.namedItem("system");
    QDomNodeList systemChildren  = system.childNodes();
    for (unsigned int k = 0; k < systemChildren.length(); k++) {
        if ("warmup"==systemChildren.at(k).nodeName().toLower()) {
            warmup = systemChildren.at(k).toElement().text().split(";");
        }else if ("cooldown"==systemChildren.at(k).nodeName().toLower()){
            cooldown = systemChildren.at(k).toElement().text().split(";");
        }
    }


}



QString VirtualPrinter::getErrors(){

    QString returnstring = "";
    QTextStream ss(&returnstring,QIODevice::WriteOnly);
    foreach(Bay* b,bays) {
        QString berr = b->getErrors();
        if(!berr.isEmpty()){
            ss<<"\nBay "<<b->getId()<<":"<<b->getErrors();
        }
    }

    if(!error_string_.isEmpty()){
        ss<<"\nVM:"<<error_string_;
    }


    return returnstring;
}

void VirtualPrinter::runCmds(QStringList sl){
//    qDebug()<<sl;
    if(printing){
        ai_->addToQueue(sl);
    }else{
        ai_->writeCommands(sl);
    }
}

bool VirtualPrinter::moveTo(double x, double y, double z, double speed){
    QStringList sl = xyzmotion->pathTo(x,y,z,speed);
    runCmds(sl);
    return true;
}
bool VirtualPrinter::move(double x, double y, double z, double speed){
    QStringList sl = xyzmotion->pathTo(x,y,z,speed,true);
    runCmds(sl);
    return true;
}

bool VirtualPrinter::forceStop(){
    ai_->estop();
    ai_->disconnect();
    initialized_ = false;
    return true;
}
void VirtualPrinter::resetPosition(){
    QString cmd = "G92 X0 Y0 Z0";
    foreach (Bay* b,bays){
        foreach(QString aname,b->actuatorNames_)
            cmd.append(" "+aname+"0");
    }
    QStringList sl;
    sl.append(cmd);
    runCmds(sl);
//    ai_->writeCommands(sl);
}


void VirtualPrinter::startprint(){
    VMPrototype::startprint();
    ai_->startQueue();
}
void VirtualPrinter::cancelprint(){
    VMPrototype::cancelprint();
    ai_->stopQueue();
    ai_->clearQueue();
}
void VirtualPrinter::pauseprint(){
    VMPrototype::pauseprint();
    ai_->stopQueue();
}

int VirtualPrinter::getBufferSize(){
    return buffsize;
}

void VirtualPrinter::setbuffsize(int s){
    buffsize = s;
}


///////////////////////////////////////////////////////

TestPrinter::TestPrinter():VMPrototype() {
}

void TestPrinter::loadConfig(QDomDocument document) {
    VMPrototype::loadConfig(document);
//    QDomElement root = document.documentElement();

//    // XYZMOTION
//    QDomNode motion = root.namedItem("motion");
//    xyzmotion =new XYZMotion(motion);

//    //BAYS
//    QDomNode tools = root.namedItem("tool");
//    QDomNodeList toolChildren  = tools.childNodes();

//    bays.clear();
//    bays = QList<Bay*>();

//    for(uint k=0; k<toolChildren.length();k++){
//        if ("bay"==toolChildren.at(k).nodeName().toLower()){
//            bays.append(new Bay(toolChildren.at(k)));
//            bays.last()->setEngine(makeEngine());
//        }
//    }

//    QDomNode system = root.namedItem("system");
//    QDomNodeList systemChildren  = system.childNodes();
//    for (unsigned int k = 0; k < systemChildren.length(); k++) {
//        if ("warmup"==systemChildren.at(k).nodeName().toLower()) {
//            warmup = systemChildren.at(k).toElement().text().split(";");
//        }else if ("cooldown"==systemChildren.at(k).nodeName().toLower()){
//            cooldown = systemChildren.at(k).toElement().text().split(";");
//        }
//    }
}





void TestPrinter::runCmds(QStringList sl){
//    totalprintcommands_+=sl;
    foreach(QString s,sl){
        s=s.simplified();
        s=s.remove("\t");
        totalprintcommands_.append(s);
//        qDebug()<<s;
    }
}
bool TestPrinter::moveTo(double x, double y, double z, double speed){
    QStringList sl = xyzmotion->pathTo(x,y,z,speed);
    runCmds(sl);
    return true;
}
bool TestPrinter::move(double x, double y, double z, double speed){
    QStringList sl = xyzmotion->pathTo(x,y,z,speed,true);
    runCmds(sl);
    return true;
}

bool TestPrinter::forceStop(){}
void TestPrinter::resetPosition(){
    QString cmd = "G92 X0 Y0 Z0";
    foreach (Bay* b,bays){
        foreach(QString aname,b->actuatorNames_)
            cmd.append(" "+aname+"0");
    }
    QStringList sl;
    sl.append(cmd);
    runCmds(sl);
}

void TestPrinter::dumpstates(){
    QFile f("dump.gcode");
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"could not open file";
        foreach(QString s, totalprintcommands_){
            qDebug()<<s;
        }
        return;
    }
    qDebug()<<"opened";
    QTextStream out(&f);
    foreach(QString s, totalprintcommands_){
        out<<"\n"<<s;
    }
    f.close();
    qDebug()<<"Wrote cmds";

}




QString runScript(VirtualPrinter *vm, QString script_) {
    QString error="";
    QTextStream ss(&error,QIODevice::ReadOnly);

    QScriptEngine* engine = vm->makeEngine();

    // Add motors to engine environment
    QString name="";
    QTextStream ns(&name,QIODevice::WriteOnly);
//    QScriptValue motorArray = engine->newArray(vm->eInterface.getMotors().size());
//    engine->globalObject().setProperty("motors",motorArray);
//    foreach(Motor* m, vm->eInterface.getMotors()) {
//        name.clear();
//        QScriptValue scriptmotor = engine->newQObject(m);
//        ns << "" << m->getID();
//        motorArray.setProperty(name,scriptmotor);
//    }

    // Add coordinated motion object to engine
//    JsCmotion jscm_;
//    jscm_.setCmotion(vm->eInterface.getCoordinatedMotion());
//    QScriptValue scriptCMotion = engine->newQObject(&jscm_);
//    engine->globalObject().setProperty("cmotion",scriptCMotion);

    if (engine->checkSyntax(script_).state()==QScriptSyntaxCheckResult::Error) {
       ss<<engine->checkSyntax(script_).errorMessage();
    }

    engine->evaluate(script_);

    if (engine->hasUncaughtException()) {
       ss<<engine->uncaughtException().toString();
    }
    return error;
}
