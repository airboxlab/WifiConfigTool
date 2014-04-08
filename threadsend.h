#ifndef TESTTHREADSEND_H
#define TESTTHREADSEND_H
#define BAUDRATE 115200

#include <QThread>
#include <QSerialPort>
#include <QMainWindow>
#include <QLineEdit>
#include <QTimer>
#include <QSerialPortInfo>
#include <QtSerialPort>
#include <QSerialPort>

class ThreadSend : public QThread
{
    Q_OBJECT
public:
    explicit ThreadSend(QObject *parent = 0);
    QByteArray wait_for_response(int msec);
    void closeSerialPort();
    void TryConnect(QString str);
protected :
    void run();
signals:
    void write(int a);
private:
    QSerialPort *serial;

public slots:
    void test(QString port,QByteArray ssid,QByteArray pwd, QByteArray encryption);
};

#endif // TESTTHREADSEND_H
