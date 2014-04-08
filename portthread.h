#ifndef PORTTHREAD_H
#define PORTTHREAD_H

#include <QThread>

class portthread : public QThread
{
    Q_OBJECT
public:
    explicit portthread(QObject *parent = 0);

signals:

public slots:

};

#endif // PORTTHREAD_H
