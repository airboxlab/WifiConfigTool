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

protected :
    void run();
signals:
    void updateList(QStringList *wifi,QStringList *encryption);
private :
    QStringList *ListWifi;
    QStringList *ListEncryption;
    QProcess *process;
    //public slots:
    //    void test();
};

#endif // WIFITHREAD_H
