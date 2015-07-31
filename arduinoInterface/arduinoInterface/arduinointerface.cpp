#include "arduinointerface.h"

#include <QDebug>
#include <stdint.h>
#include <stdio.h>

ArduinoInterface::ArduinoInterface(QObject *parent) :
    QObject(parent),current_line(0), previous_line(""),run_queue_(false),receivedBuffer(""),start_received(false)
{
    port_ = new QextSerialPort();
    connect(port_, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    readTimer = new QTimer();
    readTimer->setInterval(10);
    connect(readTimer,SIGNAL(timeout()),this,SLOT(readCheck()));
    readTimer->start();
}

ArduinoInterface::ArduinoInterface(QString port, BaudRateType baudrate, QObject *parent):
    QObject(parent),current_line(0), previous_line(""),run_queue_(false),receivedBuffer(""),start_received(false)
{
    port_ = new QextSerialPort(port);//,QextSerialPort::Polling
    port_->setBaudRate( baudrate);
    port_->setFlowControl(FLOW_OFF);
    port_->setParity(PAR_NONE);
    port_->open(QIODevice::ReadWrite);
    //    port_->set
    qDebug()<<port_->isOpen();
    connect(port_, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    readTimer = new QTimer();
    readTimer->setInterval(10);
    connect(readTimer,SIGNAL(timeout()),this,SLOT(readCheck()));
    readTimer->start();
    //_write( QString("M110") );
}
ArduinoInterface::~ArduinoInterface(){
    port_->close();
    delete port_;
}


int ArduinoInterface::queuesize(){
    return printqueue_.size();
}

bool ArduinoInterface::isReady(){
    return port_->isOpen();
}


bool ArduinoInterface::connectPort(QString port, BaudRateType baudrate){
    if (port_->isOpen()){port_->close();}
    delete port_;
    port_ = new QextSerialPort(port);
    port_->setBaudRate(baudrate);
    port_->setFlowControl(FLOW_OFF);
    port_->setParity(PAR_NONE);
    port_->open(QIODevice::ReadWrite);
#ifdef DEBUGGING
    qDebug()<<port_->isOpen();
#endif
    return port_->isOpen();
}
void ArduinoInterface::disconnect(){
    port_->close();
    qDebug()<<port_->isOpen();
}

void ArduinoInterface::addToQueue(QStringList sl){
    bool runstart = printqueue_.isEmpty();
    foreach(QString s,sl){
        printqueue_.append(s);
    }
    if(runstart){
        startQueue();
    }
    emit queuesize(printqueue_.size());
}
void ArduinoInterface::writeCommands(QStringList sl){
    if(run_queue_ || !start_received){//
        priorityqueue +=sl;
        qDebug()<<"queued"<<sl;
        qDebug()<<run_queue_;
        qDebug()<<start_received;
    }else{
        foreach(QString s, sl){
            _write(s);
        }
    }
}

void ArduinoInterface::stopQueue(){
    run_queue_=false;
}
void ArduinoInterface::startQueue(){
    if(printqueue_.isEmpty()){return;}
    run_queue_=true;
    _write(printqueue_.first());
    printqueue_.pop_front();
}

void ArduinoInterface::estop(){
    _write("M112");
    //port_->close();
    //port_->open(QIODevice::ReadWrite);
    connectPort(port_->portName(),port_->baudRate());

}

void ArduinoInterface::clearQueue(){
    printqueue_.clear();
    stopQueue();
    emit queuesize(printqueue_.size());
}

int ArduinoInterface::checksum(QString s){
    int cs = 0;
    int i = 0;
    if(s.size()>0){
        for(i = 0;i<s.size(); i++){
            if(s.at(i)=='*'){
                break;
            }
           cs = cs ^ s.at(i).toLatin1();
        }
        cs &= 0xff;
    }
    return cs;
}
void ArduinoInterface::_write_next(){
    if(!priorityqueue.isEmpty()){
        _write(priorityqueue.takeFirst());
#ifdef DEBUGGING
        qDebug()<<"***************running from priority********************\n";
#endif
    }else if(!printqueue_.isEmpty()){
        _write(printqueue_.takeFirst());
        emit queuesize(printqueue_.size());
    }
}


void ArduinoInterface::_write(QString s){
    if (!isReady() ){return;}

    previous_line = s;
    s=s.simplified();
    s = "N" + QString::number(++current_line)+" "+s;
    s+="*"+QString::number(checksum(s))+"\n";
#ifdef DEBUGGING
    qDebug()<<"writing:"<<s;
#endif
    QByteArray ba = s.toStdString().c_str();
    port_->write(ba,ba.length());


}

void ArduinoInterface::readCheck(){
//    qDebug()<<"checked, found "<<port_->bytesAvailable();
    if(port_->bytesAvailable()>0){
        onDataAvailable();
    }

}


void ArduinoInterface::onDataAvailable(){
    QByteArray data = port_->readAll();
    QString c = QString(data).toLower();
    c= c.simplified();
#ifdef DEBUGGING
    qDebug()<< "received:" <<c;
    qDebug()<< "buffer:" <<receivedBuffer;
#endif
    //c = receivedBuffer.append(c);
    //c=c.simplified();
    c=c.remove("\n");
    c = QString(data).toLower();
#ifdef DEBUGGING
    qDebug()<<c;
#endif

    if (c.contains("start")){
        start_received = true;
        _write("M110 ");
        receivedBuffer.clear();
    }else if ( c.contains("resend") || c.contains("checksum") || c.contains("error")){
        --current_line;
#ifdef DEBUGGING
        qDebug()<<c << current_line;
#endif
        _write(previous_line);

        receivedBuffer.clear();
    }else if(c.contains("o")){ // || c.contains("wait")
        _write_next();
#ifdef DEBUGGING
        qDebug()<<c;
#endif
        receivedBuffer.clear();
    }else if( c.contains("w")){
        _write_next();
        receivedBuffer.clear();
//        qDebug()<<"!!!!!w!!!!";
    }else {
        //receivedBuffer.append(c);
#ifdef DEBUGGING
        qDebug()<<c;
#endif
    }
}
