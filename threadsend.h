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

//This thread is used to run the sending configuration operation when needed
class ThreadSend : public QThread
{
    Q_OBJECT
public:
    explicit ThreadSend(QObject *parent = 0);
    //Wait for a response from the port for msec ms
    QByteArray wait_for_response(int msec);
    //Close the serial port and exit the thread
    void closeSerialPort();
    //Try to connect to the airboxlab
    bool TryConnect(QString str);
protected:
    void run();
signals:
    //Emit to the MainWindow with a number representating a recorded message
    void write(int a);
private:
    QSerialPort *serial;

public slots:
    //Send the configuration through serial port
    void SendConf(QString port,QByteArray ssid,QByteArray pwd, QByteArray encryption);
    //When the airboxlab can't connect to the AP, but the user choose to save the conf
    void writeConf(bool b);
    //Call closeSerialPort()
    void CloseConnec();
};

#endif // TESTTHREADSEND_H
