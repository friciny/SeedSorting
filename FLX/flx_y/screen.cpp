#include "screen.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeData rtdata;
extern RealtimeCmd rtcmd;

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

        this->msleep(80);
    }
}


void Screen::readScreenData()
{
    mutex.lock();
    readBuff = com_screen->readAll();
    mutex.unlock();

    if(readBuff.isEmpty())
       ;// qDebug() << "ScreenData is Empty";
    else
    {
        qDebug() << "ScreenData is" <<readBuff.toHex();
        //resolve the readBuff to rtcmd
        rtcmd = resolveCmd(readBuff);
    }

    qDebug() <<"wind"<<rtcmd.wind_freq;
    qDebug() <<"shake"<<rtcmd.shake_freq;
    qDebug() <<"feeding"<<rtcmd.feeding_door;
    qDebug() <<"front"<<rtcmd.front_door;
    qDebug() <<"back"<<rtcmd.back_door;
    qDebug() <<"wind_ctrl"<<rtcmd.wind_motor_ctrl;
    qDebug() <<"shake_ctrl"<<rtcmd.shake_motor_ctrl;
    qDebug() <<"feeding_ctrl"<<rtcmd.feeding_door_ctrl;
    qDebug() <<"front_ctrl"<<rtcmd.front_door_ctrl;
    qDebug() <<"back_ctrl"<<rtcmd.back_door_ctrl;
    qDebug() <<"stopflag" <<rtcmd.stopflag;

}


void Screen::writeScreenData()
{
    writeBuff.resize(7);

    writeBuff[0] = (char)SCREEN_RUN+'0';
    writeBuff[1] = (char)DATA_SEND+'0';
    writeBuff[2] = (char)rtdata.wind_freq+'0';
    writeBuff[3] = (char)rtdata.shake_freq+'0';
    writeBuff[4] = (char)rtdata.feeding_door+'0';
    writeBuff[5] = (char)rtdata.front_door+'0';
    writeBuff[6] = (char)rtdata.back_door+'0';

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
    qint8 addr;
    qint8 cmd_id;
    qint8 cmd_addr;

    cmd = rtcmd; //must to do

    pointer = buff.data();
    addr = *pointer++;
    qDebug() <<"addr" <<addr;
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
            case 6:
                cmd.back_door_ctrl = *pointer;
                break;
            default:
                break;
            }

        }
        else if(cmd_id == CMD_INIT)
        {
            cmd.wind_freq = *pointer++;
            cmd.shake_freq = *pointer++;
            cmd.feeding_door = *pointer++;
            cmd.front_door = *pointer++;
            cmd.back_door = *pointer;

            cmd.stopflag = false;
        }
        else if(cmd_id == CMD_CTRL-1)
        {
            cmd.feeding_door_ctrl = 0;
            cmd.front_door_ctrl = 0;
            cmd.back_door_ctrl = 0;
        }
    }
    else if(addr == SCREEN_ENTR)
    {
        cmd.wind_motor_ctrl = true;
        cmd.shake_motor_ctrl = true;
    }
    else if (addr == SCREEN_STOP)
    {
        cmd.wind_motor_ctrl = *pointer++;
        cmd.shake_motor_ctrl = *pointer++ ;//20150901
        cmd.wind_freq = 0;
        cmd.shake_freq = 0;
        cmd.feeding_door = 0;
        cmd.front_door = 0;
        cmd.back_door = 0;
        cmd.feeding_door_ctrl = 0;
        cmd.front_door_ctrl = 0;
        cmd.back_door_ctrl = 0;
        cmd.stopflag = true;
    }

    pointer = NULL;
    return cmd;
}
