#ifndef PRINTSETUPWIDGET_H
#define PRINTSETUPWIDGET_H

#include <QWidget>
#include "FabConLib/coreinterface.h"

namespace Ui {
class PrintSetupWidget;
}

class PrintSetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PrintSetupWidget(QWidget *parent, CoreInterface *ci);
    ~PrintSetupWidget();

public slots:
    void setHasFile(bool hasfile);

signals:
    void startPrint();
    void disconnectFromPrinter();


private slots:
    void on_disconnectButton_clicked();
    void on_startPrintButton_clicked();

private:
    Ui::PrintSetupWidget *ui;
    CoreInterface *ci_;
    bool hasFile_;
};

#endif // PRINTSETUPWIDGET_H
