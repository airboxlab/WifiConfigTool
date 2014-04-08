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
    t2=new ThreadSend();
    t2->start();
    statusBar()->showMessage("To begin, select the COM Port");
    connect(ui->None,SIGNAL(toggled(bool)),this,SLOT(noEncryption(bool)));
    connect(ui->update,SIGNAL(clicked()),this,SLOT(sendConfig()));
    connect(this,SIGNAL(SendMessage(QString,QByteArray,QByteArray,QByteArray)),t2,SLOT(test(QString,QByteArray,QByteArray,QByteArray)));
    connect(t2,SIGNAL(write(int)),this,SLOT(write(int)));
    connect(t1,SIGNAL(updateList(QStringList)),this,SLOT(UpdateList(QStringList)));
    connect(this,SIGNAL(SaveConf(bool)),t2,SLOT(writeConf(bool)));
}

void MainWindow::write(int a)
{
    QString m;
    switch(a)
    {
    case 0:
        m="In config Mode";
        break;
    case 1:
        m="Sending configuration to Alima";
        break;
    case 2:
        m="Connected";
        break;
    case 3:
        m="Error connection";
        break;
    case 4:
        m="Configuration saved";
        break;
    case 5:
        m="Ok";
        break;
    case 6:
        m="Error";
        break;
    case 7:
    {
        m="";
        int ret=QMessageBox::information(this, "Configuration saved","Configuration saved successfully");
        if( ret == QMessageBox::Ok ) qApp->quit();
        break;
    }

    case 10:
        {
        m="";

        int reponse = QMessageBox::warning(this, "Could not connect", "COULD NOT CONNECT :Do you want to save the configuration anyway?",QMessageBox::Yes|QMessageBox::No);
        if (reponse==QMessageBox::Yes)
        {
            emit SaveConf(true);
        }
        else emit SaveConf(false);
    }
        break;
    case 20:
        m="Entering in config mode";
        break;
    case 21:
        m="Starting new configuration";
        break;
    case 22:
        m="Waiting for information validation";
        break;
    case 23:
        m="Saving configuration";
        break;
    default:
        m="No message saved";
        break;
        //}

    }
    ui->statusBar->showMessage(m);

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

void MainWindow::sendConfig()
{
    //MainWindow::closeSerialPort();
    if (t2->isFinished())
    {
        t2->start();
    }
    if (checkData())
    {
        //TryConnect();
        QString ssid=ui->ssid->text();
        QString pwd=ui->pwd->text();
        QByteArray SSID=ssid.toUtf8();
        QByteArray PWD=pwd.toUtf8();
        QByteArray ENCRYPTION=getIndex().toUtf8();
        emit SendMessage(ui->portConnectedChoice->currentText(),SSID,PWD,ENCRYPTION);
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

QString MainWindow::getIndex()
{
    QString i="0";
    if (ui->None->isChecked())
    {
        i="0";
    }
    else if (ui->WEP->isChecked())
    {
        i="1";
    }
    else if (ui->WPA->isChecked())
    {
        i="2";
    }
    else if (ui->WPA2->isChecked())
    {
        i="3";
    }
    return i;
}

void MainWindow::closeSerialPort()
{
    serial->close();
    //ui->statusBar->showMessage(tr("Disconnected"));
}
