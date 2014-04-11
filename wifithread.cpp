#include "wifithread.h"

WifiThread::WifiThread(QObject *parent) :
    QThread(parent)
{
    ListWifi = new QStringList();
    ListEncryption = new QStringList();
   // sh= QProcess(this);
    setParent(0);
    moveToThread(this);
}

void WifiThread::run()
{
    QString *str=new QString ();
    QStringList *str1=new QStringList();
    QStringRef qref;
    QString temp;
    int sizeList=0;
    while (true)
    {
        sizeList=ListWifi->length();
        ListWifi=new QStringList();
        ListEncryption= new QStringList();
        QProcess sh;
#if defined(Q_OS_MAC)
        sh.start("sh", QStringList() << "-c" << "/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s");
        //sh.write("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s");
        sh.waitForFinished();
        QByteArray output = sh.readAll();
        sh.close();


        QStringList strl=QString(output).split(QRegExp("\n\|\r\n\|\r"));
        QStringList strl2;
        //ui->display->setText(output);
        int leng=strl.length();
        int i=0;
        QString temp;
        foreach (QString q,strl)
        {
            //ui->display->setText(ui->command->text()+"\n"+q);
            if (q.contains("WPA") && !q.contains("WPA2"))
            {
                ListEncryption->append("WPA");
            }
            else if (q.contains("WPA2"))
            {
                ListEncryption->append("WPA2");
            }
            else if(q.contains("WEP"))
            {
                ListEncryption->append("WEP");
            }
            else if(q.contains("NONE"))
            {
                ListEncryption->append("None");
            }
            strl2=q.split(":");
            if (strl2.length()>1)
            {
                ListWifi->append(cleanString(strl2.at(0)));
            }
            //ui->display->append(strl2);
            //ui->comboBox->addItem(QString::number(strl2.indexOf(regExMacAddress)));
        }
#elif defined(Q_OS_WIN)
        sh.start("netsh wlan show network");
        sh.waitForFinished();
        *str=QString(sh.readAllStandardOutput());
        *str1=str->split(QRegExp("\n\|\r\n\|\r"));
        foreach (QString q,*str1)
        {
            if((q.contains("SSID")))
            {
                //test
                //
                temp=q.split(":").at(1);
                temp=temp.mid(1,temp.length());
                ListWifi->append(temp);
            }
            if ((q.contains("Authentification")))
            {
                temp=q.split(":").at(1);
                temp=temp.mid(1,4);
                if (temp=="WPA2")
                {
                    temp="WPA2";
                }
                else if (temp=="WPA-")
                {
                    temp="WPA";
                }
                else if (temp=="WEP ")
                {
                    temp="WEP";
                }
                else //For english
                {
                    temp="None";
                }
                ListEncryption->append(temp);
            }

        }
#endif
        if (sizeList!=ListWifi->length())
        {
            emit updateList(ListWifi,ListEncryption);
        }

        QThread::msleep(1000);
    }
    exec();
}


QString WifiThread::cleanString(QString q)
{
    int i=0;
    //QChar *test=new QChar(0);
    QString result=q;
    //int length=q.length()
    int t=q.toStdString().find_first_not_of(" \t");
    int length=q.size()-t;
    return q.mid(t,q.length()-t-3);
    // else return " ";
}



