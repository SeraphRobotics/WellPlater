#include "printwidget.h"
#include "ui_printwidget.h"

PrintWidget::PrintWidget(QWidget *parent, CoreInterface *ci) :
    QWidget(parent),
    ui(new Ui::PrintWidget),
    currentPath(0),
    totalPaths(0),
    isPaused(false)
{
    ui->setupUi(this);

    ci_ = ci;

    //connect(this,SIGNAL(go()),ci_,SLOT(startPrint()));
    connect(this,SIGNAL(pause()),ci_,SLOT(pausePrint()));
    connect(this,SIGNAL(stop()),ci_,SLOT(forceStop()));
    connect(this,SIGNAL(resume()),ci_,SLOT(resumePrint()));
    connect(this,SIGNAL(cancel()),ci_,SLOT(cancelPrint()));

    connect(ci_,SIGNAL(currentCommand(int)),this,SLOT(setCurrentPath(int)));
    connect(ci_,SIGNAL(estimated(double,double,int)),this,SLOT(estimated(double,double,int)));
    connect(ci_,SIGNAL(printsComplete()),this,SLOT(printDone()));

}

PrintWidget::~PrintWidget()
{
    delete ui;
}

void PrintWidget::printDone(){
    ui->label_info->setText("Done Printing");
    currentPath = 0;
    totalPaths = 0;
    isPaused = false;
    ui->pauseButton->setText("Pause");
}

void PrintWidget::estimated(double time, double volume, int numCmd){
    totalPaths = numCmd;
    ui->totalCmdSpinBox->setValue(numCmd);
    ui->progressBar->setRange(0,totalPaths);
}

// public slots
void PrintWidget::setCurrentPath(int num){
    currentPath = num;
    ui->label_info->setText("Printing ... ");
    ui->cmdSpinBox->setValue(currentPath);
    ui->progressBar->setValue(num);
}

void PrintWidget::setTotalPaths(int num){
    totalPaths = num;
    ui->totalCmdSpinBox->setValue(num);
    ui->progressBar->setRange(0,totalPaths);
}


void PrintWidget::on_cancelButton_clicked(){
    // Cancelling the print
    ui->label_info->setText("Printing canceled");
    emit cancel();
}

void PrintWidget::on_pauseButton_clicked(){
    if (!isPaused){// pausing the print
        emit pause();
        setPaused();
    }else{
        emit resume();
        ui->label_info->setText("Resuming");
        ui->pauseButton->setText("Pause");
        isPaused=false;
    }
}

void PrintWidget::on_stopButton_clicked(){
    //CALLED for forced stop, system is printing and isnt paused
    emit stop();
    ui->label_info->setText("FORCED STOP Please reconnect to the printer.");
}

void PrintWidget::getPrinterProgress(int currPath, QString status)
{
    currentPath = currPath;
    ui->label_info->setText(status);
    ui->cmdSpinBox->setValue(currPath);
    ui->progressBar->setValue(currPath);
}


void PrintWidget::setPaused(){
    ui->pauseButton->setText("Resume");
    ui->label_info->setText("Paused");
    isPaused=true;
}

