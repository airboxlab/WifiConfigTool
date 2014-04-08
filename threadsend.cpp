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
        exit();
    }
    else
    {
        emit write(0);

        //displayError(m_readData[0]);
    //ui->statusBar->showMessage(m_readData);
    serial->write(ssid);
    serial->write("\r");
    serial->write(pwd);
    serial->write("\r");
    serial->write(encryption);

    m_readData=wait_for_response(5000);
    //ui->statusBar->showMessage("Sending conf");
    m_readData=wait_for_response(120000);
    //displayError(m_readData[0]);
    if (m_readData[0]=='0')
    {
        //ui->statusBar->showMessage("Ok ... Saving config");
        m_readData = wait_for_response(10000);
        if (m_readData[0] == '0')
        {
            emit write(4);
           // ui->statusBar->showMessage("Conf saved");
        }
    }
    /*
    else if (m_readData[0]=='4' || m_readData[0]=='b' || m_readData[0]=='8' || m_readData[0]=='9'|| m_readData[0]=='a')
    {
        //int reponse = QMessageBox::warning(this, "Could not connect", "COULD NOT CONNECT :Do you want to save the configuration anyway?",QMessageBox::Yes|QMessageBox::No);
        //if (reponse==QMessageBox::Yes)
        //{
            serial->write("s");
        //}
        //else serial->write("n");
        //m_readData=wait_for_response(5000);
        if (m_readData[0]=='0')
        {
            ui->statusBar->showMessage("No error");
        }
        else ui->statusBar->showMessage("Error");


    }
    */
    }
    closeSerialPort();

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

        if (serial->open(QIODevice::ReadWrite)) {
            if (serial->setBaudRate(BAUDRATE))
            {
               emit write(2);

            }
            } else {
                serial->close();
                emit write(3);
            }

}
