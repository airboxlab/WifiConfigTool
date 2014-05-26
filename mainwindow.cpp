#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    //Setting up the thread
    t1=new PortThread();
    t1->start();
    t2=new ThreadSend();
    t2->start();
    t3=new WifiThread();
    t3->start();
    this->centralWidget()->setLayout(ui->verticalLayout_3);

    //manualMode == true -> in manual mode
    manualMode=(bool*)malloc(sizeof(bool));
    //automatic mode by default
    contextChanged(true);
    //Not sending
    sending=false;
    encryptlist=new QStringList();
    //All the connection
    connect(ui->update,SIGNAL(clicked()),this,SLOT(sendConfig()));
    connect(this,SIGNAL(SendMessage(QString,QByteArray,QByteArray,QByteArray)),t2,SLOT(SendConf(QString,QByteArray,QByteArray,QByteArray)));
    connect(t2,SIGNAL(write(int)),this,SLOT(write(int)));
    connect(t1,SIGNAL(updateName(QString)),this,SLOT(UpdatePort(QString)));
    connect(this,SIGNAL(SaveConf(bool)),t2,SLOT(writeConf(bool)));
    connect(t1,SIGNAL(isPlugged(bool)),this,SLOT(connectedAirbox(bool)));
    connect(t3,SIGNAL(updateList(QStringList*,QStringList*)),this,SLOT(updateSSIDList(QStringList*,QStringList*)));
    connect(ui->detect,SIGNAL(toggled(bool)),this,SLOT(contextChanged(bool)));
    connect(t3,SIGNAL(emptyList(bool)),this,SLOT(emptyList(bool)));


}

//Change the contect automatically if no wifi found (true-> change to manual)
void MainWindow::emptyList(bool b)
{

    if (b)
    {
        if (!(*manualMode==b))
        {
            contextChanged(!b);
        }
        ui->detect->setText("Detect WiFi signal (No signal found)");
        ui->detect->setChecked(false);
        ui->detect->setCheckable(false);
    }
    else
    {
        ui->detect->setCheckable(true);
        ui->detect->setText("Detect WiFi signal");
    }
    qDebug() << "emptylist" << *manualMode;
}

//true ->  automatic
//toggled -> manual=false
//Called if the context changed (manual-automatic)
void MainWindow::contextChanged(bool n)
{
    int changed=0;
    qDebug() << n;
    qDebug() << "manual" << *manualMode;
    if (!n)
    {
        *manualMode=true;
    }
    else *manualMode=false;
    //Switching the view
    ui->comboBox->setVisible(false);
    ui->encryption->setVisible(false);
    ui->ssid->setVisible(false);
    ui->comboBox->setVisible(n);
    ui->encryption->setVisible(!n);
    ui->ssid->setVisible(!n);
}

//Update the SSID List found by the wifi thread
void MainWindow::updateSSIDList(QStringList *ssid,QStringList *encryption)
{
    encryptlist=encryption;
    //Store which item is select before updating
    QString temp=ui->comboBox->currentText();
    ui->comboBox->clear();

    ui->comboBox->addItems(*ssid);
    //To keep the index on the one chose by the user
    if (ssid->contains(temp))
    {
        ui->comboBox->setCurrentText(temp);
    }
}

//Write messages from the wifi thread on the status bar
void MainWindow::write(int a){
    //If error (a>=10), close connection, thread)
    if (a>=10 && a!=13)
    {
        emit CloseConnection();
    }
    QString m;
    switch(a)
    {
    case 1:
        m="Connecting to serial...";
        break;
    case 2:
        m="Entering configuration mode...";
        break;
    case 3:
        m="Sending new configuration...";
        break;
    case 4:
        m="Waiting for information...";
        break;
    case 5:
        m="Saving...";
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

//Update the port name where the airboxlab is connected
void MainWindow::UpdatePort(QString q)
{
    portConnectedName=q;
}


MainWindow::~MainWindow()
{
    free(manualMode);
    delete ui;
}

//Called if the airboxlab is connect
void MainWindow::connectedAirbox(bool b)
{
    //Not updating if sending
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
        connected=!enabled;
        lockui(enabled);
    }
}


//Lock the UI when needed (sending, no airboxlab)

void MainWindow::lockui(bool b)
{
    qDebug() << "lockUi" << *manualMode;
    //Check the mode
    if (*manualMode)
    {
        //Disable the pwd when needed
        if (ui->None->isChecked() || sending || !connected)
        {
            ui->pwd->setDisabled(true);
            if (ui->None->isChecked())
            {
                ui->pwd->clear();
            }
        }
        else
        {
            ui->pwd->setDisabled(false);
        }
        //Enable or disable the ui
        ui->update->setDisabled(b);
        ui->ssid->setDisabled(b);
        ui->encryption->setDisabled(b);
        ui->detect->setDisabled(b);
    }
    else if (encryptlist->length()!=0)
    {
        //Get the encryption mode for the selected SSID
       qDebug() << ui->comboBox->currentIndex();
        if (getIndex(ui->comboBox->currentIndex())=="0" || sending || !connected)
        {
            ui->pwd->setDisabled(true);
            if (getIndex(ui->comboBox->currentIndex())=="0")
            {
                ui->pwd->clear();
            }
        }
        else
        {
            ui->pwd->setDisabled(false);
        }
        ui->update->setDisabled(b);
        ui->comboBox->setDisabled(b);
        ui->detect->setDisabled(b);
    }
}

//Check the data before sending it depending on the mode
bool MainWindow::checkData()
{
    //In manual mode, we check if all data are entered (ssid, pwd if needed)
    if(*manualMode)
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
    //In automatic, we check that the pwd is entered, if needed
    else if (ui->pwd->text()==NULL && ui->pwd->isEnabled())
    {
        QMessageBox::warning(
                    this,
                    tr("Application Name"),
                    tr("Empty password") );
        return false;
    }
    else return true;
}


//Send all the connection info
void MainWindow::sendConfig()
{
    //We are starting the sending process
    sending=true;

    if (t2->isFinished())
    {
        t2->start();
    }
    QString ssid;
    QString pwd;
    QString encrypt;

    //Check if the data are ok
    if (checkData())
    {
        //We get the data depending on the mode
        if (*manualMode)
        {
            lockui(true);
            ssid=ui->ssid->text();
            pwd=ui->pwd->text();
            encrypt=getIndex(1);
        }
        else
        {
            //TryConnect();
            lockui(true);
            ssid=ui->comboBox->currentText();
            pwd=ui->pwd->text();
            encrypt=getIndex(ui->comboBox->currentIndex());
        }
        QByteArray SSID=ssid.toUtf8();
        QByteArray PWD=pwd.toUtf8();
        QByteArray ENCRYPTION=encrypt.toUtf8();
        qDebug() << "ssid" << SSID;
        qDebug() << "encryption" << ENCRYPTION;
        //We send it to the send thread
        emit SendMessage(portConnectedName,SSID,PWD,ENCRYPTION);
    }
}

//Get the encryption type and return the corresponding number
QString MainWindow::getIndex(int nssid)
{
    QString i="0";
    QString encrypt;
    qDebug() << "getIndex" << *manualMode;
    qDebug() << ui->comboBox->currentIndex();
    qDebug() << ui->comboBox->count();
    qDebug() << encryptlist->length();
    if (*manualMode)
    {
        if (ui->None->isChecked())
        {
            encrypt="None";
        }
        else if(ui->WEP->isChecked())
        {
            encrypt="WEP";
        }
        else if(ui->WPA->isChecked())
        {
            encrypt="WPA";
        }
        else if(ui->WPA2->isChecked())
        {
            encrypt="WPA2";
        }
    }
    else encrypt=encryptlist->at(ui->comboBox->currentIndex());
    if (encrypt=="None")
    {
        i="0";
    }
    else if (encrypt=="WEP")
    {
        i="1";
    }
    else if (encrypt=="WPA")
    {
        i="2";
    }
    else if (encrypt=="WPA2")
    {
        i="3";
    }
    return i;
}

