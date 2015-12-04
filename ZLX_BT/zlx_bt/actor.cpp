#include "actor.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeData rtdata;
extern RealtimeCmd rtcmd;

Actor::Actor(QObject *parent) :
    QThread(parent)
{
}

Actor::Actor(Posix_QextSerialPort *com)
{
    this->com_actor = com;
}


void Actor::run()
{
    while(1)
    {
        syncmutex.lock();
//        writeSensorCmd();

        writeActorCmd();

        syncmutex.unlock();

        this->msleep(50);
    }
}


void Actor::readSensorData()
{
//    this->com_actor->setParity(PAR_NONE);
    mutex.lock();
    readBuff = com_actor->readAll();
    mutex.unlock();

    if(readBuff.isEmpty())
        qDebug() << "SensorData is Empty";
    else
        qDebug() << "SensorData is" <<readBuff.toHex();
        //resolve the readBuff to rtdata
//        rtdata = resolvedata(readBuff);

    this->msleep(50);

}

void Actor::writeActorCmd()
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff ("0");

    tempcmdbuff = resolvecmd(rtcmd,rtdata);

    writeBuff[0] = CTRL_BORD_ADDR;
    writeBuff[1] = 0xA9;
    writeBuff[2] = tempcmdbuff[0];
    writeBuff[3] = 0x00;

    crcresult = CRC16(writeBuff,4);
    crclow = crcresult & 0x00ff;
    crchigh = (crcresult&0xff00)>>8;

    writeBuff.resize(6);
    writeBuff[4] = crclow;
    writeBuff[5] = crchigh;

    mutex.lock();
    com_actor->flush();
    com_actor->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

    qDebug() << "writeActorcmd = " << writeBuff.toHex();

    writeBuff.clear();

     readSensorData();
//    this->msleep(200);

}

void Actor::writeSensorCmd()
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff ("0");

    tempcmdbuff[0] = SENSOR_BORD_ADDR;
    tempcmdbuff[1] = 0x02;

    crcresult = CRC16(tempcmdbuff,2);
    crclow = crcresult & 0x00ff;
    crchigh = (crcresult&0xff00)>>8;

    writeBuff.resize(4);
    writeBuff[0] = tempcmdbuff[0];
    writeBuff[1] = tempcmdbuff[1];
    writeBuff[2] = crclow;
    writeBuff[3] = crchigh;

    mutex.lock();
    com_actor->flush();
    com_actor->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

    qDebug() << "writeSensorcmd = " << writeBuff.toHex();
    writeBuff.clear();

    readSensorData();
    this->msleep(50);


}


QByteArray Actor::resolvecmd(const RealtimeCmd &cmd, const RealtimeData &data)
{
    QByteArray cmdbuff ("0");

    //windmotor
    if(cmd.wind_motor_ctrl == true)
    {
        cmdbuff[0] = cmdbuff[0] | WINDMOTOR_OPEN;
    }

    //shakemotor
    if(cmd.shake_motor_ctrl == true)
    {
        cmdbuff[0] = cmdbuff[0] | SHAKEMOTOR_OPEN;
    }

  /*  //feedingdoor
    if((cmd.feeding_door == data.feeding_door)||(cmd.feeding_door_ctrl == STOP))
    {
         cmdbuff[0] = cmdbuff[0] & FEEDING_STOP;
    }
    else if((cmd.feeding_door<data.feeding_door)||(cmd.feeding_door_ctrl == REVERSE))
    {
        cmdbuff[0] = cmdbuff[0] | FEEDING_REVERSE;
    }
    else if((cmd.feeding_door>data.feeding_door)||(cmd.feeding_door_ctrl == FORWARD))
    {
        cmdbuff[0] = cmdbuff[0] | FEEDING_FORWARD;
    }

    //frontdoor
    if((cmd.front_door == data.front_door))
    {
        cmdbuff[0] = cmdbuff[0] & FRONTDOOR_STOP;
    }
    else if((cmd.front_door<data.front_door))
    {
        cmdbuff[0] = cmdbuff[0] | FRONTDOOR_REVERSE;
    }
    else  if((cmd.front_door>data.front_door))
    {
        cmdbuff[0] = cmdbuff[0] | FRONTDOOR_FORWARD;
    }

    //backdoor
    if((cmd.back_door == data.back_door))
    {
        cmdbuff[0] = cmdbuff[0] & BACKDOOR_STOP;
    }
    else if((cmd.back_door<data.back_door))
    {
        cmdbuff[0] = cmdbuff[0] | BACKDOOR_REVERSE;
    }
    else  if((cmd.back_door>data.back_door))
    {
        cmdbuff[0] = cmdbuff[0] | BACKDOOR_FORWARD;
    }*/


    //feedingdoor
    if(cmd.feeding_door_ctrl == STOP)
    {
         cmdbuff[0] = cmdbuff[0] & FEEDING_STOP;
    }
    else if(cmd.feeding_door_ctrl == REVERSE)
    {
        cmdbuff[0] = 0xd0;
    }
    else if(cmd.feeding_door_ctrl == FORWARD)
    {
        cmdbuff[0] = 0xe0;
    }

    //frontdoor
    if(cmd.front_door_ctrl == STOP)
    {
        cmdbuff[0] = cmdbuff[0] & FRONTDOOR_STOP;
    }
    else if(cmd.front_door_ctrl == REVERSE)
    {
        cmdbuff[0] = cmdbuff[0] | FRONTDOOR_REVERSE;
    }
    else  if(cmd.front_door_ctrl == FORWARD)
    {
        cmdbuff[0] = cmdbuff[0] | FRONTDOOR_FORWARD;
    }

//    //backdoor
//    if(cmd.back_door_ctrl == STOP)
//    {
//        cmdbuff[0] = cmdbuff[0] & BACKDOOR_STOP;
//    }
//    else if(cmd.back_door_ctrl == REVERSE)
//    {
//        cmdbuff[0] = cmdbuff[0] | BACKDOOR_REVERSE;
//    }
//    else  if(cmd.back_door_ctrl == FORWARD)
//    {
//        cmdbuff[0] = cmdbuff[0] | BACKDOOR_FORWARD;
//    }

    return cmdbuff;
}

RealtimeData Actor::resolvedata(QByteArray &buff)
{
    RealtimeData data;
    char *pointer;
    quint8 cmd_id;
    quint8 addr;

    data = rtdata;

    pointer = buff.data()+1;
    addr = *pointer++;
    cmd_id = *pointer++;



    if(addr == 0x24)
    {
        qDebug() <<"addr" <<addr;
        qDebug() <<"cmd_id = " <<cmd_id;
        data.front_door = cmd_id;
        data.back_door = *(pointer+1);
    }

    if((addr == 0x04)&&(cmd_id == 0x24))
    {
        qDebug() <<"addr" <<addr;
        qDebug() <<"cmd_id = " <<cmd_id;
        data.front_door = *pointer;
        data.back_door=*(pointer+2);
    }



    return data;

}
