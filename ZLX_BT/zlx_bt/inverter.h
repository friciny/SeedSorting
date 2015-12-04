#ifndef INVERTER_H
#define INVERTER_H

#include "global.h"

#define PZD1_H_WRITE   0x04
#define PZD1_H_READ     0x00
#define PZD1_L_STOP       0x7E
#define PZD1_L_RUN        0x7F
#define PZD2_H_STOP      0x00
#define PZD2_L_STOP       0x00

class Inverter : public QThread
{
    Q_OBJECT
public:
    explicit Inverter(QObject *parent = 0);
    Inverter(Posix_QextSerialPort*,int);
    ~Inverter();
    void readInverterData();   
    void setInverterFreq();
    void getInverterFreq();

private:
    Posix_QextSerialPort *com_inverter;
    QByteArray readBuff;
    QByteArray writeBuff;
    int InverterAddr;

signals:

protected:
    void run();
    void init();
    void stop();
    void writeInverterFreq(qint8 );
    void writeInverterCmd(quint8 ,quint8 ,quint8 ,quint8);
    RealtimeData resolveInvData(QByteArray&);

    
public slots:
    
};

#endif // INVERTER_H
