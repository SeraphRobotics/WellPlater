#include "baywidget.h"
#include "ui_baywidget.h"
#include "baydialog.h"
#include <iostream>


BayWidget::BayWidget(QWidget *parent, CoreInterface *ci,int id):
    QWidget(parent), ui(new Ui::BayWidget)
{
    ui->setupUi(this);
    id_=id;

    QString bayName;
    QTextStream bs(&bayName,QIODevice::WriteOnly);
    bs<<"Bay "<<id;
    ui->bayLabel->setText(bayName);

    ci_=ci;

    //Create baydialog for this bay
    dialog = new BayDialog(this,id,ci);
    connect(ci_,SIGNAL(materialsAvailable(QMap<int,Material>)),this,SLOT(setMaterials(QMap<int,Material>)));

}

void BayWidget::setMaterials(QMap<int,Material> materials){
    QStringList materialList;
    materialList.append(QString());

    materialMap_.clear();
    idtomaterials_.clear();

    QMapIterator<int,Material> i(materials);
    while (i.hasNext()){
        i.next();
        Material m = i.value();
        QString name = m.name.toLower();
        if (materialList.contains(name)){
            QTextStream ns(&name,QIODevice::WriteOnly);
            ns<<m.id;
        }
        materialMap_[i.key()]= name ;
        idtomaterials_[name]=i.key();
        materialList.append(name);
    }
    ui->materialCombo->clear();
    ui->materialCombo->addItems(materialList);
}


void BayWidget::on_editButton_clicked()
{
    dialog->show();
}

BayWidget::~  BayWidget()
{
    delete ui;
}


void BayWidget::setBayCommand(int bayNum, double distance)
{
    ci_->moveBayMotor(bayNum,distance,distance);/// THIS IS BAD FORM: TODO add a time/speed mechanism
}

void BayWidget::on_materialCombo_activated(const QString &arg1)
{
    QString material = arg1;
    if(idtomaterials_.contains(material)){
        int matid = idtomaterials_[material];
        qDebug()<<"Material in bay "<<id_<<" set to "<<matid;
        ci_->setMaterial(id_,matid);
    }
}
