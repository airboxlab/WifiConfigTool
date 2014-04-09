#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    /*
    serial=new QSerialPort();
    */
    ui->setupUi(this);
    t1=new PortThread();
    t1->start();
    t2=new ThreadSend();
    t2->start();
    sending=false;
    statusBar()->showMessage("To begin, select the COM Port");
    connect(ui->None,SIGNAL(toggled(bool)),this,SLOT(noEncryption(bool)));
    connect(ui->update,SIGNAL(clicked()),this,SLOT(sendConfig()));
    connect(this,SIGNAL(SendMessage(QString,QByteArray,QByteArray,QByteArray)),t2,SLOT(test(QString,QByteArray,QByteArray,QByteArray)));
    connect(t2,SIGNAL(write(int)),this,SLOT(write(int)));
    connect(t1,SIGNAL(updateName(QString)),this,SLOT(UpdateList(QString)));
    connect(this,SIGNAL(SaveConf(bool)),t2,SLOT(writeConf(bool)));
    connect(t1,SIGNAL(isPlugged(bool)),this,SLOT(connectedAirbox(bool)));
}

void MainWindow::write(int a)
{
    //If error (a>=10), close connection, thread)
    if (a>=10 && a!=13)
    {
        emit CloseConnection();
    }
    QString m;
    switch(a)
    {
    case 1:
        m="Connecting to serial";
        break;
    case 2:
        m="Entering configuration mode";
        break;
    case 3:
        m="Sending new configuration";
        break;
    case 4:
        m="Waiting for information";
        break;
    case 5:
        m="Saving";
        break;
    case 6:
    {
        m="";
        int ret=QMessageBox::information(this, "Configuration saved","Configuration saved successfully");
        if( ret == QMessageBox::Ok ) qApp->quit();
        break;
    }
    case 10:
    {
        m="";
        int reponse = QMessageBox::warning(this, "Could not connect", "Can't connect to the device, please try again");
        sending=false;
        lockui(false);
        break;
    }
    case 11:
    {
        m="";
        int reponse = QMessageBox::warning(this, "Configuration mode", "Error while entering configuration mode, please try again");
        sending=false;
        lockui(false);
        break;
    }
    case 12:
    {
        m="";
        int reponse = QMessageBox::warning(this, "Configuration mode", "Error while sending configuration, please try again");
        sending=false;
        lockui(false);
        break;
    }
    case 13:
    {
        m="";
        int reponse = QMessageBox::warning(this, "Could not connect", "AIRBOXLAB COULD NOT CONNECT TO THE WIRELESS SIGNAL(Time out in 60 sec) Do you want WiFi credentials to be saved in Airboxlab anyway?",QMessageBox::Yes|QMessageBox::No);
        if (reponse==QMessageBox::Yes)
        {
            emit SaveConf(true);
        }
        else emit SaveConf(false);
        break;
    }
    case 14:
    {
        m="";
        int ret=QMessageBox::information(this, "Configuration not saved","Could not confirm information, no saving, please try again");
        sending=false;
        lockui(false);
        //if( ret == QMessageBox::Ok ) qApp->quit();
        break;
    }
    case 15:
    {
        m="";
        int ret=QMessageBox::information(this, "Can't save","Error while saving, please try again");
        sending=false;
        lockui(false);
        //if( ret == QMessageBox::Ok ) qApp->quit();
        break;
    }
    case 16:
    {
        m="";
        int ret=QMessageBox::information(this, "Configuration not saved","Error while writing no saving");
        sending=false;
        lockui(false);
        //if( ret == QMessageBox::Ok ) qApp->quit();
        break;
    }
    default:
        m="No message saved";
        break;
        //}

    }

    ui->statusBar->showMessage(m);

}
void MainWindow::UpdateList(QString q)
{
    portConnectedName=q;
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

void MainWindow::connectedAirbox(bool b)
{
    if (!sending)
    {
        bool enabled=false;
        if (b)
        {
            ui->statusBar->showMessage("Airboxlab detected");
            enabled=false;
        }
        else
        {
            ui->statusBar->showMessage("Airboxlab not detected : Please plug it to the computer");
            enabled=true;
        }
        lockui(enabled);
    }
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

void MainWindow::lockui(bool b)
{
    ui->encryption->setDisabled(b);
    ui->ssid->setDisabled(b);
    ui->pwd->setDisabled(b);
    ui->update->setDisabled(b);
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
    sending=true;
    lockui(true);
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
        emit SendMessage(portConnectedName,SSID,PWD,ENCRYPTION);
    }
}

/*
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
*/

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

/*
void MainWindow::closeSerialPort()
{
    serial->close();
    //ui->statusBar->showMessage(tr("Disconnected"));
}
*/
