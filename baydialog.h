#ifndef BAYDIALOG_H
#define BAYDIALOG_H

#include <QDialog>
#include "FabConLib/coreinterface.h"

namespace Ui {
    class BayDialog;
}

class BayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BayDialog(QWidget *parent, int id, CoreInterface *cin);
    ~BayDialog();

signals:
    void sendBayCommand(int bayNum, double distance, bool absolute);

private slots:

    void on_incrementSpin_editingFinished();
    void on_commandSpin_editingFinished();
    void on_downButton_clicked();
    void on_upButton_clicked();
    void on_moveButton_clicked();
    void on_acceptButton_clicked();

private:
    Ui::BayDialog *ui;
    int id_;
    CoreInterface *ci;
};

#endif // BAYDIALOG_H
