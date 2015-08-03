#include "baydialog.h"
#include "ui_baydialog.h"
#include "baywidget.h"

BayDialog::BayDialog(QWidget *parent, int id, CoreInterface *cin) :
    QDialog(parent),
    ui(new Ui::BayDialog)
{
    ci = cin;
    ui->setupUi(this);
    id_ = id;

    QString bayName;
    QTextStream bs(&bayName,QIODevice::WriteOnly);
    bs << "Bay "<<id;

    ui->bayLabel->setText(bayName);
    ui->incrementSpin->setValue(0.20);
    ui->acceptButton->setFocus();
    this->setWindowTitle("Edit Bay");
}

BayDialog::~BayDialog()
{
    delete ui;
}

void BayDialog::on_moveButton_clicked()
{  
    ci->moveBayMotor(id_,ui->commandSpin->value(),ui->commandSpin->value() );/// THIS IS BAD FORM: TODO add a time/speed mechanism
    ui->positionSpin->setValue(ui->commandSpin->value());
}

void BayDialog::on_acceptButton_clicked()
{
    this->close();
}


void BayDialog::on_upButton_clicked()
{

    double increment = ui->incrementSpin->value();
    double position = ui->positionSpin->value();

    ci->moveBayMotor(id_,-increment,increment);/// THIS IS BAD FORM: TODO add a time/speed mechanism
    ui->positionSpin->setValue(position-increment);
}

void BayDialog::on_downButton_clicked()
{
    double increment = ui->incrementSpin->value();
    double position = ui->positionSpin->value();
//    qDebug()<<"down button";
    ci->moveBayMotor(id_,increment,increment);/// THIS IS BAD FORM: TODO add a time/speed mechanism

    ui->positionSpin->setValue(position+increment);
}

void BayDialog::on_commandSpin_editingFinished()
{
    this->setFocus();
}

void BayDialog::on_incrementSpin_editingFinished()
{
    this->setFocus();
}
