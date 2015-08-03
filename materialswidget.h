#ifndef MATERIALSWIDGET_H
#define MATERIALSWIDGET_H

#include <QWidget>
#include "baywidget.h"
#include "FabConLib/coreinterface.h"

using namespace std;

namespace Ui {
    class MaterialsWidget;
}

class MaterialsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialsWidget(QWidget *parent, CoreInterface *ci);

    ~MaterialsWidget();

public slots:
    void updateBays();
    void cleanUpBays();
    void setMaterials(QMap<int,Material> materials);
    void materialNeeded(int i);
    void updateState(int i);

private:
    Ui::MaterialsWidget *ui;
    int numBays;
    QVector<BayWidget*> bayWidgets;
    CoreInterface *ci_;
    QMap<int,Material> materials_;

};

#endif // MATERIALSWIDGET_H
