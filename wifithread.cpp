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
        sh.start("sh", QStringList() << "-c" << "/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s");
        //sh.write("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s");
        sh.waitForFinished();
        QByteArray output = sh.readAll(); //sssss
        sh.close();
//#endif

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



