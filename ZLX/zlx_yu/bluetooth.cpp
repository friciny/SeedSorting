#include "bluetooth.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeCmd rtcmd;

Bluetooth::Bluetooth(QObject *parent) :
    QThread(parent)
{
}

Bluetooth::Bluetooth(Posix_QextSerialPort *com)
{
    this->com_Bluetooth = com;

}

void Bluetooth::run()
{
    while(1)
    {
        syncmutex.lock();

        readBluetoothData();

        syncmutex.unlock();

        this->msleep(150);
    }
}

void Bluetooth::readBluetoothData()
{
    mutex.lock();

    readBuff = com_Bluetooth->readAll();

    mutex.unlock();

    if(!readBuff.isEmpty())
    {
        rtcmd = resolveCmd(readBuff);
    }
    else
        qDebug()<<"BlueToothData is Empty";

}

RealtimeCmd Bluetooth::resolveCmd(QByteArray &buff)
{
    RealtimeCmd cmd;
    char *pointer;
    qint8 addr;
    qint8 cmd_id;

    cmd = rtcmd;

    pointer = buff.data();
    addr = *pointer++;
    qDebug()<<"addr = "<<addr;
    cmd_id = *pointer++;

    return cmd;
}
