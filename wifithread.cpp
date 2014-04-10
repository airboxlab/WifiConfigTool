#include "wifithread.h"

WifiThread::WifiThread(QObject *parent) :
    QThread(parent)
{
    ListWifi = new QStringList();
    ListEncryption = new QStringList();
    process = new QProcess(this);
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
        temp=ListWifi->length();
        ListWifi=new QStringList();
        ListEncryption= new QStringList();
        #if defined(Q_OS_WIN)
                process->start("netsh wlan show network");
#endif
        process->waitForFinished();
        *str=QString(process->readAllStandardOutput());
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
                else if (temp=="Ouvr" || temp=="Open") //For english
                {
                    temp="None";
                }
                ListEncryption->append(temp);
            }

        }
        if (sizeList!=ListWifi->length())
        {
            emit updateList(ListWifi,ListEncryption);
        }
        exec();
        QThread::msleep(1000);
    }

}
