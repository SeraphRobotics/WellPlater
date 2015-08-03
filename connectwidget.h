#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>

#include <QWidget>
#include <QHash>
#include <QFileInfoList>
#include <QDir>
#include "FabConLib/coreinterface.h"
#include "qextserialenumerator.h"
namespace Ui
{
    class ConnectWidget;
}

class ConnectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWidget(QWidget *parent, CoreInterface *ci);
    ~ConnectWidget();

public slots:
    void loading(bool load);
    void reLoadConfigFiles();
    void deviceAdded(QextPortInfo i);
    void deviceRemoved(QextPortInfo i);
    void updateSlots();

signals:
    void atemptConnect();
    void mainDeviceRemoved();

private:
    void loadFiles();
    void addConfig(QString path);
    void removeConfig(QString path);
    Ui::ConnectWidget *ui;
    CoreInterface *ci_;
    QStringList portList;
    QString portName;
    int portItem;
    QFileInfoList configList;
    QDir configFileDirectory;
    QString default_config_path;
    QFileInfo configFile;
    QTimer* t_;
    QList<QextPortInfo> allports;

private slots:
    void on_connectButton_clicked();
};
#endif // CONNECTWIDGET_H
