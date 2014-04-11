#ifndef WIFITHREAD_H
#define WIFITHREAD_H

#include <QThread>
#include <QStringList>
#include <QProcess>

class WifiThread : public QThread
{
    Q_OBJECT
public:
    explicit WifiThread(QObject *parent = 0);
    QString cleanString(QString q);

protected :
    void run();
signals:
    void updateList(QStringList *wifi,QStringList *encryption);
private :
    QStringList *ListWifi;
    QStringList *ListEncryption;
    QProcess *sh; //public slots:
    //    void test();
};

#endif // WIFITHREAD_H
