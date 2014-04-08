#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    serialPortInfoList = QSerialPortInfo::availablePorts();
    serial=new QSerialPort();
    ui->setupUi(this);
    //Initialize the list of COM port
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
    {
        if (serialPortInfo.portName()!="Bluetooth-Incoming-Port" && serialPortInfo.portName()!="Bluetooth-Modem")
        {
        ui->portConnectedChoice->addItem(*(new QString(serialPortInfo.portName())));
        }
    }

    t1=new PortThread();
    t1->start();
    //Disable all the line edit
    //ui->ssid->setEnabled(false);
    //ui->pwd->setEnabled(false);
    //ui->encryption->setEnabled(false);
    //Inform the user what to do
    statusBar()->showMessage("To begin, select the COM Port");
    //About the info on the GUI
    //connect(ui->portConnectedChoice,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeStatusBar()));
    //connect(ui->portConnectedChoice,SIGNAL())
    //connect(ui->portConnectedChoice,SIGNAL(),this,SLOT(UnlockWifiParameter()));
    connect(ui->None,SIGNAL(toggled(bool)),this,SLOT(noEncryption(bool)));
    //connect(ui->update,SIGNAL(clicked()),this,SLOT(checkData()));
    //About the serial communication
    //connect(ui->update,SIGNAL(clicked()),this,SLOT(TryConnect()));
    connect(ui->update,SIGNAL(clicked()),this,SLOT(sendConfig()));
    //connect(ui->update,SIGNAL(clicked()),t2,SLOT(test()));
    //connect(t2,SIGNAL(finished()),this,SLOT(Write()));
    connect(t1,SIGNAL(updateList(QStringList)),this,SLOT(UpdateList(QStringList)));
}

void MainWindow::Write()
{
    ui->statusBar->showMessage("from thread");
}

void MainWindow::UpdateList(QStringList q)
{
    ui->portConnectedChoice->clear();
    ui->portConnectedChoice->addItems(q);
}

void MainWindow::noEncryption(bool b)
{
    ui->pwd->clear();
    ui->pwd->setDisabled(b);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Not used yet
void MainWindow::UpdatePort()
{
    serialPortInfoList = QSerialPortInfo::availablePorts();
    ClearView();
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
    {
        ui->portConnectedChoice->addItem(*(new QString(serialPortInfo.portName())));
    }

}

void MainWindow::ClearView()
{
    ui->portConnectedChoice->clear();
    ui->portConnectedChoice->update();
}

void MainWindow::ChangeStatusBar()
{
    statusBar()->showMessage("Enter the WiFi parameter");
}

void MainWindow::UnlockWifiParameter()
{
    ui->ssid->setEnabled(true);
    ui->pwd->setEnabled(true);
    ui->encryption->setEnabled(true);

}

bool MainWindow::checkData()
{
    if (ui->ssid->text()==NULL && ((!ui->None->isChecked()) && ui->pwd->text()==NULL) )
    {
        QMessageBox::warning(
            this,
            tr("Application Name"),
            tr("No SSID & No password") );
        return false;
    }
    else if ((!ui->None->isChecked()) && ui->pwd->text()==NULL)
    {
        QMessageBox::warning(
            this,
            tr("Application Name"),
            tr("Empty password") );
        return false;
    }
    else if (ui->ssid->text()==NULL)
    {
        QMessageBox::warning(
            this,
            tr("Application Name"),
            tr("No SSID") );
        return false;
    }
    else return true;
}

void MainWindow::displayError(const char val)
{
   QString str;
    switch (val)
    {
    case '0':
        str="Ok";
        break;
    case '1':
        str="SSID Length";
        break;
    case '2':
        str="Pass length";
        break;
    case '3':
        str="Encrypt mode";
        break;
    case '4':
        str="NEW AP";
        break;
    case '5':
        str="Error saving";
        break;
    case '6':
        str="Timeout";
        break;
    case '7':
        str="no config saved to restore";
        break;
    case '8':
        str="could not get an IP";
        break;
    case '9':
        str="could not get the adress of the backend";
        break;
    case 'a':
        str="could not ping the backend";
        break;
    case 'b':
        str="timeout2";
        break;
    case 'c':
        str="settinf profile fail";
        break;
    }
    ui->statusBar->showMessage(str);

}

void MainWindow::TryConnect()
{
    serial->setPortName(ui->portConnectedChoice->currentText());
    //if there is no open serial communication
    if(serial->isOpen())
    {
        closeSerialPort();
    }

        if (serial->open(QIODevice::ReadWrite)) {
            if (serial->setBaudRate(BAUDRATE))
            {
                ui->statusBar->showMessage(tr("Connected to"));

            }
            } else {
                serial->close();
                QMessageBox::critical(this, tr("Error"), serial->errorString());

                ui->statusBar->showMessage(tr("Open error"));
            }

}


void MainWindow::sendConfig()
{
    //MainWindow::closeSerialPort();
    if (checkData())
    {
             TryConnect();
            QMessageBox::information(this,"connected","connected");
            QString ssid=ui->ssid->text();
            QString pwd=ui->pwd->text();
            QString encrypt=QString::number(getIndex());
            QByteArray SSID=ssid.toUtf8();
            QByteArray PWD=pwd.toUtf8();
            //QByteArray ENCRYPT=encrypt.toUtf8();
            //ui->statusBar->showMessage(SSID);
            //ui->statusBar->showMessage("Starting config mode");
            serial->write("w");

            m_readData=wait_for_response(5000);
            displayError(m_readData[0]);
            //ui->statusBar->showMessage(m_readData);
            serial->write(SSID);
            serial->write("\r");
            serial->write(PWD);
            serial->write("\r");

            if (ui->None->isChecked())
            {
                serial->write("0");
            }
            else if (ui->WEP->isChecked())
            {
                serial->write("1");
            }
            else if (ui->WPA->isChecked())
            {
                serial->write("2");
            }
            else if (ui->WPA2->isChecked())
            {
                serial->write("3");
            }
            m_readData=wait_for_response(5000);
            ui->statusBar->showMessage("Sending conf");
            m_readData=wait_for_response(120000);
            displayError(m_readData[0]);
            if (m_readData[0]=='0')
            {
                ui->statusBar->showMessage("Ok ... Saving config");
                m_readData = wait_for_response(10000);
                if (m_readData[0] == '0')
                {
                    ui->statusBar->showMessage("Conf saved");
                }
            }
            else if (m_readData[0]=='4' || m_readData[0]=='b' || m_readData[0]=='8' || m_readData[0]=='9'|| m_readData[0]=='a')
            {
                int reponse = QMessageBox::warning(this, "Could not connect", "COULD NOT CONNECT :Do you want to save the configuration anyway?",QMessageBox::Yes|QMessageBox::No);
                if (reponse==QMessageBox::Yes)
                {
                    serial->write("s");
                }
                else serial->write("n");
                m_readData=wait_for_response(5000);
                if (m_readData[0]=='0')
                {
                    ui->statusBar->showMessage("No error");
                }
                else ui->statusBar->showMessage("Error");


            }
            closeSerialPort();
    }
}

QByteArray MainWindow::wait_for_response(int msec)
{
    bool b=true;
    QByteArray tmp;
    serial->waitForReadyRead(msec);
    while (b)
    {
        tmp=serial->read(1);
        ui->statusBar->showMessage(tmp);
        if (tmp=="#")
        {
            b=false;
        }
    }
    return serial->readAll();

}

int MainWindow::getIndex()
{
    int i=0;
    if (ui->None->isChecked())
    {
        i=0;
    }
    else if (ui->WEP->isChecked())
    {
        i=1;
    }
    else if (ui->WPA->isChecked())
    {
        i=2;
    }
    else if (ui->WPA2->isChecked())
    {
        i=3;
    }
    return i;
}

void MainWindow::closeSerialPort()
{
    serial->close();
    //ui->statusBar->showMessage(tr("Disconnected"));
}
