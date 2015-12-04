#include "screen.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeData rtdata;
extern RealtimeCmd rtcmd;
extern RealtimeCount  rtcount;

Screen::Screen(QObject *parent) :
    QThread(parent)
{
}


Screen::Screen(Posix_QextSerialPort *com)
{
    this->com_screen = com;
}

void Screen::run()
{
    while(1)
    {
        syncmutex.lock();

        readScreenData();
        writeScreenData();
        syncmutex.unlock();

        this->msleep(100);
    }
}


void Screen::readScreenData()
{
    mutex.lock();
    readBuff = com_screen->readAll();
    mutex.unlock();

    if(readBuff.isEmpty())
       qDebug() << "ScreenData is Empty";
    else
    {
        qDebug() << "ScreenData is" <<readBuff.toHex();
     }
    //resolve the readBuff to rtcmd
      rtcmd = resolveCmd(readBuff);

}


void Screen::writeScreenData()
{
    writeBuff.resize(8);

    writeBuff[0] = (uchar)DATA_SEND;
    writeBuff[1] = (uchar)rtdata.temphigh;
    writeBuff[2] = (uchar)rtdata.templow;
    writeBuff[3] = (uchar)rtdata.humihigh;
    writeBuff[4] = (uchar)rtdata.humilow;
    writeBuff[5] = (uchar)rtcount.CountHour;
    writeBuff[6] = (uchar)rtcount.CountMinute;
    writeBuff[7] = (uchar)rtcount.CountSecond;


    mutex.lock();
    com_screen->flush();
    com_screen->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

     qDebug()<<"writeScreenData"<<writeBuff.toHex();
     writeBuff.clear();
    this->msleep(50);
}

RealtimeCmd Screen::resolveCmd(QByteArray & buff)
{
    RealtimeCmd cmd;
    char *pointer;
    qint8 cmd_id;

    cmd = rtcmd; //must to do

    pointer = buff.data();
    cmd_id = *pointer++;
    qDebug() <<"cmd_id" <<cmd_id;

    if(cmd_id == DATA_SET)
    {
        cmd.temp_limit_high = *pointer++;
        cmd.temp_limit_low = *pointer++;
        cmd.humi_limit_high = *pointer++;
        cmd.humi_limit_low = *pointer++;
    }
    else if(cmd_id == MANUAL)
    {
        if(cmd.state != *pointer)
        {
            cmd.state = *pointer;
            rtcount.CountHour=0;
            rtcount.CountMinute=0;
            rtcount.CountSecond=0;
        }

        qDebug()<<"cmd.state="<<cmd.state;
        qDebug()<<"startcounting="<<cmd.startcount;
    }
    else if(cmd_id == AUTO)
    {
        if(cmd.state != *(pointer+1))
        {
            cmd.state = *(pointer+1);
            rtcount.CountHour=0;
            rtcount.CountMinute=0;
            rtcount.CountSecond=0;
        }
    }

    pointer = NULL;
    return cmd;
}
