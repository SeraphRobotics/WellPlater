#include "printsetupwidget.h"
#include "ui_printsetupwidget.h"

PrintSetupWidget::PrintSetupWidget(QWidget *parent, CoreInterface *ci) :
    QWidget(parent),
    ci_(ci),
    hasFile_(false),
    ui(new Ui::PrintSetupWidget)
{
    ui->setupUi(this);
    connect(this,SIGNAL(startPrint()),ci_,SLOT(startPrint()));
    connect(this,SIGNAL(disconnectFromPrinter()),ci_,SLOT(disconnectVM()));
}

PrintSetupWidget::~PrintSetupWidget()
{
    delete ui;
}

void PrintSetupWidget::setHasFile(bool hasfile){
    hasFile_=hasfile;
    if(!hasFile_){
        ui->startPrintButton->setDisabled(true);
    }else{
        ui->startPrintButton->setEnabled(true);
    }
}

void PrintSetupWidget::on_disconnectButton_clicked(){
    emit disconnectFromPrinter();
}
void PrintSetupWidget::on_startPrintButton_clicked(){
    if(hasFile_){
        emit startPrint();
    }
}
