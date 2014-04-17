#ifndef WIFITHREAD_H
#define WIFITHREAD_H

#include <QThread>
#include <QStringList>
#include <QProcess>

//This thread is used to get a list the wifi signal available
class WifiThread : public QThread
{
    Q_OBJECT
public:
    explicit WifiThread(QObject *parent = 0);
    //Get SSID from a line from the airport -s command
    QString cleanString(QString q);

protected :
    void run();
signals:
    //Emitted when the list changes
    void updateList(QStringList *wifi,QStringList *encryption);
    //Emitted when there is no wifi available
    void emptyList(bool);
private :
    //Contains the list of SSID
    QStringList *ListWifi;
    //Contains the encryption type
    QStringList *ListEncryption;
    //Process from where the cmd and terminal command are launched
    QProcess *sh;
};

#endif // WIFITHREAD_H
