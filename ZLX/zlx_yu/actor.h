#ifndef ACTOR_H
#define ACTOR_H

#include "global.h"

#define FORWARD 1
#define REVERSE    2
#define STOP            0

class Actor : public QThread
{
    Q_OBJECT
public:
    explicit Actor(QObject *parent = 0);
    Actor(Posix_QextSerialPort*);
    void readSensorData();
    void writeActorCmd();
    void writeSensorCmd();

private:
    Posix_QextSerialPort *com_actor;
    QByteArray readBuff;
    QByteArray writeBuff;


protected:
    void run();
    QByteArray resolvecmd(const RealtimeCmd&,const RealtimeData&);
    RealtimeData resolvedata(QByteArray &buff);

signals:
    
public slots:
    
};

#endif // ACTOR_H
