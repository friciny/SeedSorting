#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include "global.h"


class Bluetooth : public QThread
{
    Q_OBJECT
public:
    explicit Bluetooth(QObject *parent = 0);
    Bluetooth(Posix_QextSerialPort*);


private:
    Posix_QextSerialPort *com_Bluetooth;
    QByteArray readBuff,writeBuff;
    void readBluetoothData();
    void writeBluetoothData();
    RealtimeCmd resolveCmd(QByteArray &buff);

protected:
    void run();
    
signals:
    
public slots:
    
};

#endif // BLUETOOTH_H
