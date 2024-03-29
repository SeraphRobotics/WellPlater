#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>

#include <connectwidget.h>
#include <printwidget.h>
#include <printsetupwidget.h>
#include <materialswidget.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum ViewState{
        CONNECT,
        SETUP,
        PRINT
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void printerConnected();
    void onStateChaged(int i);
    void materialNeeded(int i);
    void printDone();
    void setPause();
    void setResume();

private slots:
    void errors(QString errs);
    void urlChanged(QUrl url);


private:
    void updateState();
    void setUpWidgets();
    void setUpConnections();
    void hideMaterialsWidget();
    void showMaterialsWidget();

private:
    Ui::MainWindow *ui;
    int current_state;
    bool haveValidFile;
    QextSerialEnumerator* enumerator;
    MaterialsWidget* mw_;
    ConnectWidget* cw_;
    PrintSetupWidget* psw_;
    PrintWidget* pw_;
    CoreInterface* ci_;
};

#endif // MAINWINDOW_H
