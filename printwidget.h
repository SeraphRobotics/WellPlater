#ifndef PRINTWIDGET_H
#define PRINTWIDGET_H
#include <QWidget>
#include "FabConLib/coreinterface.h"

namespace Ui {
    class PrintWidget;
}

class PrintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PrintWidget(QWidget *parent, CoreInterface *ci);
    ~PrintWidget();

public slots:
    void setCurrentPath(int num);
    void setTotalPaths(int num);
    void getPrinterProgress(int currPath, QString status);
    void estimated(double time, double volume, int numCmd);
    void printDone();
    void setPaused();

signals:
    void pause();
    void stop();
    void resume();
    void cancel();

private:
    Ui::PrintWidget *ui;
    CoreInterface *ci_;
    int currentPath;
    int totalPaths;
    bool isPaused;

private slots:
    void on_stopButton_clicked();
    void on_cancelButton_clicked();
    void on_pauseButton_clicked();
};
#endif // PRINTWIDGET_H
