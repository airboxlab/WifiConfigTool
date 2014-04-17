#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QThread>
#include <QSerialPortInfo>
#include <QStringList>

//This thread is used to know if the airboxlab is connected to the computer or not
class PortThread : public QThread
{
    Q_OBJECT
public:
    explicit PortThread(QObject *parent = 0);
protected :
    void run();

signals:
    //Emit when airboxlab discovered with the name of the port
    void updateName(QString);
    //Emit true if connected, false if it isn't
    void isPlugged(bool);

private :
    //Length of the list which contains the ports information
    int length;
    //Contains the ports information
    QList<QSerialPortInfo> serialPortInfoList;
};

#endif // PORTTHREAD_H
