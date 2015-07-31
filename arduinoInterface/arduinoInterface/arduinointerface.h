#ifndef ARDUINOINTERFACE_H
#define ARDUINOINTERFACE_H

#include <QObject>
#include "qextserialport.h"
#include <QVector>
#include <QStringList>
#include <QTimer>
#include <QThread>

class ArduinoInterface : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit ArduinoInterface(QObject *parent = 0);
    ArduinoInterface(QString port, BaudRateType baudrate, QObject *parent = 0);
    ~ArduinoInterface();

    bool isReady();
    int queuesize();

signals:
    void portNotOpen();
    void queuesize(int s);

public slots:
    bool connectPort(QString port, BaudRateType baudrate);
    void disconnect();
    void addToQueue(QStringList sl);
    void writeCommands(QStringList sl);
    void stopQueue();
    void startQueue();
    void clearQueue();
    void estop();


private slots:
    void _write(QString s);
    void _write_next();
    void onDataAvailable();
    void readCheck();

private:
    int checksum(QString s);

private:
    QextSerialPort* port_;
    QStringList printqueue_;
    QStringList priorityqueue;
    int current_line;
    QString previous_line;
    bool run_queue_;
    QString receivedBuffer;
    bool start_received;
    QTimer* readTimer;
};

#endif // ARDUINOINTERFACE_H
