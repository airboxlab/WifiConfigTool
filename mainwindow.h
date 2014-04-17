#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define BAUDRATE 115200

#include <QMainWindow>
#include <QLineEdit>
#include <QTimer>
#include <QSerialPortInfo>
#include <QtSerialPort>
#include <QSerialPort>
#include <QMessageBox>
#include "portthread.h"
#include "threadsend.h"
#include "wifithread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QString getIndex(int nssid);
    //Check the data before sending it
    bool checkData();
    //Lock the UI when needed (sending, no airboxlab)
    void lockui(bool);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //Contain all the available COM Port
    QList<QSerialPortInfo> serialPortInfoList;
    QTimer m_timer;
    QByteArray  m_readData;
    //Detect if the airboxlab is connected
    PortThread *t1;
    //Update credential
    ThreadSend *t2;
    //Check if wifi available and type of encryption
    WifiThread *t3;
    //true -> in manual mode
    bool *manualMode;
    //Contains all the encryption of the wifi signal available
    QStringList *encryptlist;
    //Contains the name of the serial port where the airboxlab is
    QString portConnectedName;
    //true if sending the conf (t2 slot running)
    bool sending;
    //true if the airboxlab is connected
    bool connected;

signals:
    //Send the information to perform the operation on the airboxlab
    void SendMessage(QString port,QByteArray ssid,QByteArray pwd, QByteArray encryption);
    //If the user choose to save conf when the airboxlab can't connect to the AP
    void SaveConf(bool b);
    //Send to the send thread to close the serial connection
    void CloseConnection();
    //Send to the port thread to stop
    void stopThreadPort(bool b);


public slots:
    //Lock the pwd line edit  when needed (no encryption)
    void lockPass();
    //Send all the connection info
    void sendConfig();
    //Write messages from the wifi thread on the status bar
    void write(int a);
    //Update the port name where the airboxlab is connected
    void UpdatePort(QString q);
    //Called if the airboxlab is connect
    void connectedAirbox(bool);
    //Called if the context changed (manual-automatic)
    void contextChanged(bool);
    //Update the SSID List found by the wifi thread
    void updateSSIDList(QStringList *ssid,QStringList *encryption);
    //Change the contect automatically if no wifi found (true-> change to manual)
    void emptyList(bool b);
};

#endif // MAINWINDOW_H
