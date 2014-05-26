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

//Only called when the user click on the button and the data are checked
void ThreadSend::SendConf(QString portName,QByteArray ssid,QByteArray pwd, QByteArray encryption)
{
    //We try to connect
    if (ThreadSend::TryConnect(portName))
    {
        QByteArray  m_readData;
        //We enter in config mode
        serial->write("w");
        m_readData=ThreadSend::wait_for_response(15000);
        if (m_readData[0]!='0')
        {
            emit write(11);
            CloseConnec();
        }
        else
        {
            qDebug() << (encryption.at(0)==1);
            if (encryption.at(0)=='1' && (pwd.length()==10 || pwd.length()==26))
            {
                pwd=QByteArray::fromHex(pwd);
            }
            qDebug() << encryption.at(0);
            qDebug() << "pwd ="+ pwd;
            //If we're in config mode, we send the informations
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
                    qDebug() << m_readData[0];
                    emit write(13);
                }
                else
                {
                    emit write(5);
                    m_readData = wait_for_response(10000);
                    if (m_readData[0] != '0')
                    {
                        CloseConnec();
                        emit write(15);
                    }
                    else
                    {
                        emit write(6);
                        CloseConnec();
                    }
                }
            }
        }
    }
}

//Called when the airboxlab couldn't connect to the ap but the user still want to save the configuration
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


//Wait for the response of the airboxlab for msec ms then read the response
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

//Try to connect then return true if connected, false otherwise
bool ThreadSend::TryConnect(QString str)
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
            return true;
        }
    } else {
        emit write(10);
        return false;
    }

}

void ThreadSend::closeSerialPort()
{
    serial->close();
}

void ThreadSend::CloseConnec()
{
    closeSerialPort();
    exit();
}
