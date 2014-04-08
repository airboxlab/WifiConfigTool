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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QByteArray wait_for_response(int msec);
    void displayError(const char a);
    void closeSerialPort();
    QString getIndex();
    //Check the data before sending it
    bool checkData();


    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //Contain all the available COM Port
    QList<QSerialPortInfo> serialPortInfoList;
    QTimer m_timer;
    QByteArray  m_readData;
    QSerialPort *serial;
    PortThread *t1;
    ThreadSend *t2;

signals:
    void SendMessage(QString port,QByteArray ssid,QByteArray pwd, QByteArray encryption);

public slots:
    //Update List of COM Port
    void UpdatePort();
    //Empty the combobox and the list
    void ClearView();
    //Give status on the statusbar
    void ChangeStatusBar();
    //Control which widget is enabled
    void UnlockWifiParameter();

    //Send all the connection info
    void sendConfig();
    //Connect to the device via serial
    //void TryConnect();
    void noEncryption(bool b);
    void write(int a);
    void UpdateList(QStringList q);
};

#endif // MAINWINDOW_H
