#include "portthread.h"
#include "QMessageBox"
PortThread::PortThread(QObject *parent) :
    QThread(parent)
{

    serialPortInfoList=QSerialPortInfo::availablePorts();
    QSerialPortInfo *q2=new QSerialPortInfo();
    length=serialPortInfoList.length();
    setParent(0);
    moveToThread(this);
}

void PortThread::run()
{
    bool b;
    QString name;
    //To know if it's the first iteration in the loop
    int i=0;
    //Infinite loop
    while (true)
    {
        b=false;
        QThread::msleep(500);
        serialPortInfoList=QSerialPortInfo::availablePorts();
        //We update only when there is change or if it's the first time
        //We check in each port info if the manufacter is us
            foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
            {
                if (serialPortInfo.manufacturer()=="www.airboxlab.com" || serialPortInfo.manufacturer()=="getalima.com" || serialPortInfo.manufacturer()=="http://getalima.com/" || serialPortInfo.manufacturer()=="http://www.airboxlab.com/" )
                {
                    b=true;
                    name=*(new QString(serialPortInfo.portName()));
                }

            }
            //We emit to the main thread the information
            emit isPlugged(b);
            emit updateName(name);

            //We update the length of the port list
            length=serialPortInfoList.length();
            //Not the first iteration anymore
            i=1;
    }
    exec();
}



