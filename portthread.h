#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QThread>
#include <QSerialPortInfo>
#include <QStringList>

class PortThread : public QThread
{
    Q_OBJECT
public:
    explicit PortThread(QObject *parent = 0);
protected :
    void run();
signals:
    void updateName(QString);
    void isPlugged(bool);
public slots:
    void ProcessingConf(bool);
private :
    int length;
    bool runable;
    QList<QSerialPortInfo> serialPortInfoList;
    QStringList *ListPort;
    //public slots:
    //    void test();
};

#endif // PORTTHREAD_H
