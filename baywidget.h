#ifndef BAYWIDGET_H
#define BAYWIDGET_H

#include <QWidget>
#include <qpushbutton.h>
#include "baydialog.h"
#include "FabConLib/coreinterface.h"

namespace Ui {
    class BayWidget;
}

class BayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BayWidget(QWidget *parent,
                       CoreInterface *ci,
                       int id = 0);
    ~BayWidget();

public slots:
    void setBayCommand(int bayNum, double distance);
    void setMaterials(QMap<int,Material> materials);

private slots:
    void on_editButton_clicked();
    void on_materialCombo_activated(const QString &arg1);

public:
    QPushButton editButton;
private:
    Ui::BayWidget *ui;
    BayDialog* dialog;
    int id_ ;
    CoreInterface *ci_;
    QMap<int,QString> materialMap_;
    QMap<QString,int> idtomaterials_;
};

#endif // BAYWIDGET_H
