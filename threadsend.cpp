#include "threadsend.h"


ThreadSend::ThreadSend(QObject *parent) :
    QThread(parent)
{
    serial=new QSerialPort();
    setParent(0);
    moveToThread(this);
}


void ThreadSend::run()
{
    exec();
}

void ThreadSend::test(QString portName,QByteArray ssid,QByteArray pwd, QByteArray encryption)
{
    ThreadSend::TryConnect(portName);
    QByteArray  m_readData;
    serial->write("w");
    m_readData=ThreadSend::wait_for_response(15000);
    if (m_readData[0]!='0')
    {
        emit write(11);
        CloseConnec();
    }
    else
    {
        emit write(3);
        serial->write(ssid);
        serial->write("\r");
        serial->write(pwd);
        serial->write("\r");
        serial->write(encryption);
        //wait for the device to answer that it has read everything (CONFIG_ERROR_NO_ERROR) + timeout
        m_readData=wait_for_response(5000);
        if (m_readData[0]!='0')
        {
            emit write(12);
            CloseConnec();
        }
        else
        {
            emit write(4);
            m_readData=wait_for_response(120000);
            if (m_readData[0]!='0')
            {
                emit write(13);
            }
            else
            {
                emit write(5);
                m_readData = wait_for_response(10000);
                if (m_readData[0] != '0')
                {
                    emit write(15);
                    CloseConnec();
                }
                else
                {
                    emit write(6);
                    CloseConnec();
                }
                /*
                else if (m_readData[0]=='4' || m_readData[0]=='b' || m_readData[0]=='8' || m_readData[0]=='9'|| m_readData[0]=='a')
                {
                    //Boxmessage on the GUI
                    emit write(10);
                    //int reponse = QMessageBox::warning(this, "Could not connect", "COULD NOT CONNECT :Do you want to save the configuration anyway?",QMessageBox::Yes|QMessageBox::No);
                    //if (reponse==QMessageBox::Yes)
                    //{

                    //}
                    //else serial->write("n");

                }*/
            }
        }
    }

    // closeSerialPort();

}

void ThreadSend::writeConf(bool b)
{
    QByteArray  m_readData;
    if (b)
    {
        serial->write("s");
        m_readData=wait_for_response(5000);
        emit write(5);
        m_readData = wait_for_response(10000);
        if (m_readData[0] != '0')
        {
            emit write(15);
            CloseConnec();
        }
        else
        {
            emit write(6);
            CloseConnec();
        }
    }
    else
    {
        serial->write("n");
        m_readData=wait_for_response(5000);
        if (m_readData[0]=='0')
        {
            emit write(14);
            CloseConnec();
        }
        else
        {
            emit write(16);
            CloseConnec();
        }
    }
}

void ThreadSend::closeSerialPort()
{

    serial->close();
}

QByteArray ThreadSend::wait_for_response(int msec)
{
    bool b=true;
    QByteArray tmp;
    serial->waitForReadyRead(msec);
    while (b)
    {
        tmp=serial->read(1);
        if (tmp=="#")
        {
            b=false;
        }
    }
    return serial->readAll();

}

void ThreadSend::TryConnect(QString str)
{
    serial->setPortName(str);
    //if there is no open serial communication
    if(serial->isOpen())
    {
        closeSerialPort();
    }
    emit write(1);
    if (serial->open(QIODevice::ReadWrite)) {
        if (serial->setBaudRate(BAUDRATE))
        {
            emit write(2);

        }
    } else {
        emit write(10);
    }

}

void ThreadSend::CloseConnec()
{
    closeSerialPort();
    exit();
}
