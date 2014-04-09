#include "portthread.h"
#include "QMessageBox"
PortThread::PortThread(QObject *parent) :
    QThread(parent)
{

    serialPortInfoList=QSerialPortInfo::availablePorts();
    QSerialPortInfo *q2=new QSerialPortInfo();
    ListPort=new QStringList();
    length=serialPortInfoList.length();
    setParent(0);
    moveToThread(this);
    runable=true;
}

void PortThread::run()
{
    bool b;
    QString name;
    int i=0;
    while (true)
    {
        if (runable)
        {
            b=false;
            QThread::msleep(500);
            serialPortInfoList=QSerialPortInfo::availablePorts();
            //if (length!=serialPortInfoList.length() || i==0)
            //{
            foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
            {
                if (serialPortInfo.manufacturer()=="www.airboxlab.com" || serialPortInfo.manufacturer()=="getalima.com" || serialPortInfo.manufacturer()=="http://getalima.com/" || serialPortInfo.manufacturer()=="http://www.airboxlab.com/" )
                {
                    b=true;
                    name=*(new QString(serialPortInfo.portName()));

                }
                /* If we need a list of the available port
                if (serialPortInfo.portName()!="Bluetooth-Incoming-Port" && serialPortInfo.portName()!="Bluetooth-Modem")
                {

                    ListPort->append(*(new QString(serialPortInfo.manufacturer())));
                }
                */

            }
            //length=serialPortInfoList.length();
            emit isPlugged(b);
            emit updateName(name);
            ListPort->clear();
            //}
        }
        else QThread::msleep(500);
        i=1;
    }
    exec();
}

void PortThread::ProcessingConf(bool b)
{
    runable=b;
}


