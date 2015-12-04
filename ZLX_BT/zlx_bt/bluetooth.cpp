#include "bluetooth.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeCmd rtcmd;
extern RealtimeData rtdata;

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
        writeBluetoothData();
        syncmutex.unlock();

        this->msleep(50);
    }
}

void Bluetooth::readBluetoothData()
{
    if( com_Bluetooth->bytesAvailable()>3)
    {
        mutex.lock();

        readBuff = com_Bluetooth->readAll();

        mutex.unlock();

        if(!readBuff.isEmpty())
        {
            rtcmd = resolveCmd(readBuff);
            qDebug()<<"BluetoothData is "<<readBuff.toHex();
        }
        this->msleep(20);
    }
}

void Bluetooth::writeBluetoothData()
{
    writeBuff.resize(2);

    writeBuff[0] = (uchar)rtcmd.wind_freq;
    writeBuff[1] = (uchar)rtcmd.shake_freq;

    mutex.lock();
    com_Bluetooth->flush();
    com_Bluetooth->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

    qDebug()<<"writeBluetoothData = "<<writeBuff.toHex();
    writeBuff.clear();
    this->msleep(20);
}

RealtimeCmd Bluetooth::resolveCmd(QByteArray &buff)
{
    RealtimeCmd cmd;
    char *pointer;
    qint8 addr;
    qint8 cmd_id;
    qint8 cmd_addr;

    cmd = rtcmd;

    pointer = buff.data();
    addr = *pointer++;
    qDebug()<<"addr = "<<addr;
    cmd_id = *pointer++;

    if(addr == SCREEN_RUN)
    {
        if(cmd_id == CMD_CTRL)
        {
            cmd_addr = *pointer++;
            switch(cmd_addr)
            {
            case 2:
                cmd.wind_freq = *pointer;
                break;
            case 3:
                cmd.shake_freq = *pointer;
                break;
            case 4:
                cmd.feeding_door_ctrl = *pointer;
                break;
            case 5:
                cmd.front_door_ctrl = *pointer;
                break;
//            case 6:
//                cmd.back_door_ctrl = *pointer;
//                break;
            default:
                break;
            }
        }
    }


    return cmd;
}
