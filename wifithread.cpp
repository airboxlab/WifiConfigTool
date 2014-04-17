#include "wifithread.h"

WifiThread::WifiThread(QObject *parent) :
    QThread(parent)
{
    ListWifi = new QStringList();
    ListEncryption = new QStringList();
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
    //Infinite loop
    while (true)
    {
        sizeList=ListWifi->length();
        ListWifi=new QStringList();
        ListEncryption= new QStringList();
        QProcess sh;
//On Mac Os
#if defined(Q_OS_MAC)
        //We start the aiport -s command in the terminal
        sh.start("sh", QStringList() << "-c" << "/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s");
        sh.waitForFinished();
        //We get the anwser
        QByteArray output = sh.readAll(); //sssss
        sh.close();
        //Split the string in a QStringList containing each line
        QStringList strl=QString(output).split(QRegExp("\n\|\r\n\|\r"));
        QStringList strl2;
        int leng=strl.length();
        int i=0;
        QString temp;
        //In each line
        foreach (QString q,strl)
        {
            //We search for the encryption mode
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
            //We get the ssid by splitting with ":" and extract the ssid with cleanString (terminal : SSID XX:XX:XX:XX:XX ...)
            strl2=q.split(":");
            if (strl2.length()>1)
            {
                ListWifi->append(cleanString(strl2.at(0)));
            }
        }
    //If on windows
#elif defined(Q_OS_WIN)
        //We start the netsh cmd
        sh.start("netsh wlan show network");
        sh.waitForFinished();
        //We read and split in a QStringList containing each line
        *str=QString(sh.readAllStandardOutput());
        *str1=str->split(QRegExp("\n\|\r\n\|\r"));
        //We extract the SSID and encryption mode
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
        //If there is change or there is no wifi available
        if (sizeList!=ListWifi->length() || ListWifi->length()==0)
        {
            if (ListWifi->length()==0)
            {
                emit emptyList(true);
            }
            else emit emptyList(false);
            emit updateList(ListWifi,ListEncryption);
        }

        QThread::msleep(1000);
    }
    exec();
}

//Get the SSID from a line from airport -s
QString WifiThread::cleanString(QString q)
{
    int t=q.toStdString().find_first_not_of(" \t");
    return q.mid(t,q.length()-t-3);
}



