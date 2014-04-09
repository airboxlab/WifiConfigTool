#include "portthread.h"

PortThread::PortThread(QObject *parent) :
    QThread(parent)
{

    serialPortInfoList=QSerialPortInfo::availablePorts();
    QSerialPortInfo *q2=new QSerialPortInfo();
    ListPort=new QStringList();
    length=serialPortInfoList.length();
    setParent(0);
    moveToThread(this);
}

void PortThread::run()
{
    int i=0;
    while (true)
    {
        QThread::msleep(500);
        serialPortInfoList=QSerialPortInfo::availablePorts();
        if (length!=serialPortInfoList.length() || i==0)
        {
            foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
            {

                if (serialPortInfo.portName()!="Bluetooth-Incoming-Port" && serialPortInfo.portName()!="Bluetooth-Modem")
                {

                    ListPort->append(*(new QString(serialPortInfo.portName())));
                }
            }
            length=serialPortInfoList.length();
            emit updateList(*ListPort);
            ListPort->clear();
        }
        i=1;
    }
    exec();
}


