#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QWebFrame>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    current_state(0),
    haveValidFile(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ci_ = new CoreInterface();
    enumerator = new QextSerialEnumerator();
    setUpWidgets();
    setUpConnections();
    QSettings s;
    QUrl url = s.value("home-url","http://www.SeraphRobotics.com").toUrl();
    ui->webView->setUrl(url);

}

MainWindow::~MainWindow()
{
    delete ci_;
    delete ui;
}

void MainWindow::setUpWidgets()
{
    cw_ = new ConnectWidget(this,ci_);
    psw_ = new PrintSetupWidget(this, ci_);
    pw_ = new PrintWidget(this,ci_);
    mw_ = new MaterialsWidget(this,ci_);
    this->mw_->hide();

    ui->stackedWidget->insertWidget(CONNECT,cw_);
    ui->stackedWidget->insertWidget(SETUP,psw_);
    ui->stackedWidget->insertWidget(PRINT,pw_);
    updateState();

}

void MainWindow::setUpConnections()
{
    //COMPORTS
    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)),cw_,SLOT(deviceAdded(QextPortInfo)));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)),cw_,SLOT(deviceRemoved(QextPortInfo)));
    connect(cw_,SIGNAL(mainDeviceRemoved()),this,SLOT(terminate()));
    //CoreInterface
    connect(ci_,SIGNAL(stateChaged(int)),this,SLOT(onStateChaged(int)));
    connect(ci_,SIGNAL(needMaterialLoaded(int)),this,SLOT(materialNeeded(int)));
    connect(ci_,SIGNAL(printsComplete()),this,SLOT(printDone()));
    connect(ci_,SIGNAL(error(QString)),this,SLOT(errors(QString)));

    connect(this, SIGNAL(sendReloadConfigCommand()), cw_, SLOT(reLoadConfigFiles()));

    connect(pw_, SIGNAL(pause()), this, SLOT(setPause()));
//    connect(pw_, SIGNAL(stop()), this, SLOT(setStop()));
//    connect(pw_, SIGNAL(cancel()), this, SLOT(setStop()));
    connect(pw_, SIGNAL(resume()), this, SLOT(setResume()));


    connect(ui->webView,SIGNAL(urlChanged(QUrl)),this,SLOT(urlChanged(QUrl)));
}

void MainWindow::hideMaterialsWidget(){
    this->mw_->hide();
    ui->widgetVLayout->removeWidget(this->mw_);
}

void MainWindow::showMaterialsWidget(){
    ui->widgetVLayout->addWidget(this->mw_);
    this->mw_->show();
}

void MainWindow::urlChanged(QUrl url){
    QWebFrame* theframe = ui->webView->page()->mainFrame();
    theframe->addToJavaScriptWindowObject("coreinterface",ci_);
    qDebug()<<url;
}


void MainWindow::printDone(){
    if((current_state !=SETUP)&&(current_state !=CONNECT)){
        current_state = SETUP;
        QMessageBox::information(this,"Print Complete","Your print has finished");
        updateState();
    }
}

void MainWindow::updateState()
{
    ui->stackedWidget->setCurrentIndex(current_state);
}

void MainWindow::printerConnected(){

    if (ci_->vm_->isInitialized())
    {
        qDebug()<<"Config file loaded successfully";
    }

    else
    {
        QMessageBox::information(this, "SeraphPrint", tr("Error: invalid config file"));

    }
}


void MainWindow::onStateChaged(int i){
    qDebug()<<"Machine State changed to "<<i;
    if(i==CoreInterface::NotInitialized){
        current_state=CONNECT;
        psw_->setHasFile(false);
    }else if (i==CoreInterface::Connected){
        current_state=SETUP;
        printerConnected();
        psw_->setHasFile(false);
        showMaterialsWidget();
    }else if(i==CoreInterface::FileLoaded){
        current_state=SETUP;
        qDebug()<<"File Loaded...";
        haveValidFile = true;
        psw_->setHasFile(true);
        showMaterialsWidget();
    }else if(i==CoreInterface::Printing){
        hideMaterialsWidget();
        current_state=PRINT;
        qDebug()<<"Printing...";
    }else{
        qDebug()<<"WTF"<<i;
        psw_->setHasFile(false);
    }
    updateState();
}

void MainWindow::materialNeeded(int i){
    setPause();
    pw_->setPaused();
}


void MainWindow::setPause()
{
    showMaterialsWidget();
}

void MainWindow::setResume()
{
    hideMaterialsWidget();
}

void MainWindow::errors(QString errs){
    qDebug()<<errs;
    if(!errs.isEmpty()){
        QMessageBox::warning (this,"error",errs);
    }
}


