#include "materialswidget.h"
#include "ui_materialswidget.h"
#include "baywidget.h"
#include <QMessageBox>

MaterialsWidget::MaterialsWidget(QWidget *parent, CoreInterface *ci) :
    QWidget(parent),
    ui(new Ui::MaterialsWidget),
    numBays(0)
{
    ui->setupUi(this);
    ci_ = ci;
    connect(ci_,SIGNAL(materialsAvailable(QMap<int,Material>)),this,SLOT(setMaterials(QMap<int,Material>)));
    connect(ci_,SIGNAL(needMaterialLoaded(int)),this,SLOT(materialNeeded(int)));
    connect(ci_,SIGNAL(stateChaged(int)),this,SLOT(updateState(int)));
    updateBays();

}

MaterialsWidget::~MaterialsWidget()
{
    delete ui;
    foreach(BayWidget* b,bayWidgets){delete b;}

}

void MaterialsWidget::updateState(int i){
    if (i==CoreInterface::Connected){
        updateBays();
    }
}


void MaterialsWidget::updateBays(){
    numBays = ci_->vm_->bays.size();
    qDebug() << "MaterialsWidget::updateBays() has numBays =" << numBays << ".";
    cleanUpBays();
    for (int x = 0; x < numBays; x++){
        BayWidget* b = new BayWidget(this,ci_,x);
        b->setMaterials(materials_);
        bayWidgets.append(b);
        ui->horizontalLayout->addWidget(b);
    }
}

void  MaterialsWidget::cleanUpBays(){
    foreach(BayWidget* b,bayWidgets){
        ui->horizontalLayout->removeWidget(b);
        b->hide();
        delete b;
    }
    bayWidgets.clear();
}

void MaterialsWidget::setMaterials(QMap<int,Material> materials){
    materials_ = materials;
}


void MaterialsWidget::materialNeeded(int i){
    QString message;
    QTextStream ms(&message,QIODevice::WriteOnly);
    ms<<"Print is paused since it needs a material not loaded in the machine\n";
    ms<<"Please load Material id:"<<i<<" Name: "<<materials_[i].name;
    QMessageBox::warning(this,"Material Needed",message);
}
