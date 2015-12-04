#ifndef ACTOR_H
#define ACTOR_H

#include "global.h"

class Actor : public QThread
{
    Q_OBJECT
public:
    explicit Actor(QObject *parent = 0);
    Actor(Posix_QextSerialPort*);
    void setTempLimit(int temp_high,int temp_low);
    void checkTemp();
    void getTempLimit();
    void checkHumi();

private:
    Posix_QextSerialPort *com_actor;
    QByteArray readBuff;
    QByteArray writeBuff;
    void readSensorData();
    void writeActorCmd(int buff);
    void writeTempCmd(int buff);
    void writeHumiCmd(int buff);



protected:
    void run();
    void resolvecmd(const RealtimeCmd&);
    RealtimeData resolvedata(QByteArray &buff);

signals:
    
public slots:
    
};

#endif // ACTOR_H
