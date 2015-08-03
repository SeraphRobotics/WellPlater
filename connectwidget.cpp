#include "connectwidget.h"
#include "ui_connectwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include "qextserialenumerator.h"
#include <QDebug>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>

#define FAB_CONFIG_DIRECTORY_NAME_UNIX ".printer_configs"
#define FAB_CONFIG_DIRECTORY_NAME_WIN "Printer Configs"

ConnectWidget::ConnectWidget(QWidget *parent, CoreInterface *ci) : QWidget(parent), ui(new Ui::ConnectWidget)
{
    ui->setupUi(this);
    ci_ = ci;
    t_ = new QTimer();

    // Set up default directory
    // Settings for Config file default directory path
    // Find OS specific app_data filepath
    // QSettings constructor values were specified in main.cpp.
    QSettings settings;
    default_config_path = QFileInfo(settings.fileName()).absolutePath();//+ "/Seraph"
    allports = QList<QextPortInfo>();

    QString verifyDirectoryExistsCommand;

    // Find the config directory from QSettings
    configFileDirectory = QDir(settings.value("config_dir", default_config_path).toString());

    system(verifyDirectoryExistsCommand.toStdString().c_str());

    loadFiles();

    t_->setInterval(100);
    connect(t_,SIGNAL(timeout()),this,SLOT(updateSlots()));
    t_->start(100);
    //updateSlots();


}

ConnectWidget::~ConnectWidget()
{
    delete ui;
}



// Reload .config files from the appropriate directory for the given platform
void ConnectWidget::loadFiles()
{
    configList = configFileDirectory.entryInfoList(QStringList("*.config"), QDir::Files);

    //std::cout << configList.at(0).absoluteFilePath().toStdString() << std::endl;
    // Clear the entries currently in the config combo box
    //ui->configBox->clear();

    // If there are no config files, disable deletion
    if (configList.isEmpty()) {
        ui->connectButton->setEnabled(false);
    }
    else {
        QSettings s;
        QString filename = s.value("config/filename","wellplater").toString();
        foreach(QFileInfo config, configList){
            if(config.baseName().contains(filename,Qt::CaseInsensitive)){
                ui->connectButton->setEnabled(true);
                configFile = config;
            }
        }
    }


}

void ConnectWidget::updateSlots(){

    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    if (ports.size()!= allports.size()){
        portList.clear();
        foreach(QextPortInfo port,ports )
        {
            deviceAdded(port);
        }

        foreach(QextPortInfo port,allports){
            bool innewlist = false;
            foreach(QextPortInfo newport,ports ){
                innewlist = innewlist || newport.physName==port.physName;
            }
            if (!innewlist){deviceRemoved(port);}
        }
        allports = ports;
    }
}

void ConnectWidget::deviceAdded(QextPortInfo i){
    qDebug() << "Device added named: " +i.friendName+ " on port "+ i.portName + ". Calling response code.";

    bool usb = i.friendName.contains("Arduino Mega",Qt::CaseInsensitive);

    if (usb)//!port.friendName.isEmpty())
    {
        portList.append(i.portName);
        ui->portBox->addItem(i.friendName.replace("Arduino Mega 2560","Scientist"));
        /**
            TODO: Check for duplicates
        **/
    }else{
        qDebug()<<i.friendName<<" connected";
    }
}

void ConnectWidget::deviceRemoved(QextPortInfo i){
    qDebug()<<"Device removed named: " + i.portName + ". Calling response code.";
    bool usb = i.friendName.contains("Arduino Mega",Qt::CaseInsensitive);

    if (usb)//!port.friendName.isEmpty())
    {
        //portList.removeAt(portList.indexOf(i.portName));
        ui->portBox->removeItem(ui->portBox->findText(i.friendName.replace("Arduino Mega 2560","Scientist")));
        /**
            TODO: Alert other stages to changes, display warning of some kind.
        **/
        if(i.portName == portName){
            mainDeviceRemoved();
        }
    }
}

void ConnectWidget::on_connectButton_clicked(){
    int portIndex   = ui->portBox  ->currentIndex();
    bool canConnect = true;



    if ((portIndex == -1)||(portList.isEmpty())){
        QMessageBox::information(this, "SeraphPrint", tr("Error: Select a valid COM port from the list."));
        canConnect = false;
    }


    if (canConnect){

        // LOAD THE FILE
        QString config_path = configFile.absoluteFilePath();
//        QString config_path = "JrKerr-Single-deposition.config";
        qDebug() << "Loading: " + config_path;
        QString configString;
        QDomDocument configDom;
        // load the config file into the DOM document
        {
          QFile configFile(config_path);
          if (!configFile.open(QFile::ReadOnly)) {
              qDebug() << "Failed to open config file.";
              QMessageBox::warning(this,tr("Config Error"),tr("Cound not open config file"));
              return;
          }
          configDom.setContent(&configFile);
          configFile.close();
        }

        qDebug() << "Configuring config";
        configString = configDom.toString();
        qDebug() << "Configuring port";
        portName = portList.at(portIndex);

        // ATTEMPT THE CONNECTION

        qDebug() << "Attempting connection";
        ci_->setConfig(configString,portName);
        emit atemptConnect();
        /// Need an mechanism for checking errors
    }
}

void ConnectWidget::loading(bool load){
    ui->connectButton->setEnabled(load);
    ui->portBox->setEnabled(load);
}

//reloads config files after user changes the default config file directory
void ConnectWidget::reLoadConfigFiles()
{
    // QSetting to grab config directory
    // QSettings constructor values were specified in main.cpp.
    QSettings settings;
    configFileDirectory = QDir(settings.value("config_dir", default_config_path).toString());
    loadFiles();
}
