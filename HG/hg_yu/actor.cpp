#include "actor.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeData rtdata;
extern RealtimeCmd rtcmd;
extern RealtimeCount rtcount;

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
        checkTemp();
        checkHumi();
        resolvecmd(rtcmd);
        syncmutex.unlock();

        this->msleep(200);
    }
}


void Actor::readSensorData()
{
    mutex.lock();
    readBuff = com_actor->readAll();
    mutex.unlock();

    if(readBuff.isEmpty())
        qDebug() << "SensorData is Empty";
    else
    {
        qDebug() << "SensorData is" <<readBuff.toHex();
        rtdata = resolvedata(readBuff);
    }
    this->msleep(50);
}

void Actor::writeActorCmd(int buff)
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff ("0");

    tempcmdbuff[0] = buff;

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

    this->msleep(50);
    readSensorData();

}

void Actor::writeTempCmd(int buff)
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff("0");

    tempcmdbuff[0] = TEMP_BORD_ADDR;
    tempcmdbuff[1] = 0x99;
    tempcmdbuff[2] = buff;

    crcresult = CRC16(tempcmdbuff,3);
    crclow = crcresult & 0x00ff;
    crchigh = (crcresult&0xff00)>>8;

    writeBuff.resize(5);
    writeBuff[0] = tempcmdbuff[0];
    writeBuff[1] = tempcmdbuff[1];
    writeBuff[2] = tempcmdbuff[2];
    writeBuff[3] = crclow;
    writeBuff[4] = crchigh;

    mutex.lock();
    com_actor->flush();
    com_actor->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

    qDebug() << "writeTempcmd = " << writeBuff.toHex();
    writeBuff.clear();

     this->msleep(50);
    readSensorData();
}

void Actor::writeHumiCmd(int buff)
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff ("0");

    tempcmdbuff[0] = HUMI_BORD_ADDR;
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

    qDebug() << "writeHumicmd = " << writeBuff.toHex();
    writeBuff.clear();

    this->msleep(50);
    readSensorData();

}


void Actor::resolvecmd(const RealtimeCmd &cmd)
{
    switch (cmd.state)
    {
    case 0:
        writeActorCmd(FEEDING);
        break;
    case 1:
        writeActorCmd(FEEDED);
        break;
    case 2:
        writeActorCmd(CIRCULATING);
        setTempLimit(cmd.temp_limit_high,cmd.temp_limit_low);
        break;
    case 3:
        if(((rtdata.temphigh>=rtcmd.temp_limit_high)
            &&(rtdata.templow>=rtcmd.temp_limit_low))
                ||((rtdata.humihigh<=rtcmd.humi_limit_high)
                   &&(rtdata.humilow<=rtcmd.humi_limit_low)))
        {
            writeTempCmd(TEMPCTRL_2);
        }
        else
        {
             getTempLimit();
             writeTempCmd(TEMPCTRL_1);
        }
        break;
    case 4:
        writeTempCmd(TEMPCTRL_2);
        break;
    case 5:
        writeTempCmd(TEMPCTRL_3);
        writeActorCmd(ACTORSTOP);
        break;
    }
}

RealtimeData Actor::resolvedata(QByteArray &buff)
{
    RealtimeData data;
    char *pointer;
    quint8 addr;
    quint8 cmd_id;
    quint8 data_array[6];
    quint16 data_avg;

    data = rtdata; //must to do

    pointer = buff.data();
    pointer++;
    addr = *pointer++;
    cmd_id = *pointer++;
    qDebug() <<"addr" <<addr;

    if((addr == TEMP_BORD_ADDR)&&(cmd_id == 0x27))
    {
        for(int i=0;i<6;i++)
        {
            data_array[i] = *(pointer+i+1);
        }
        data_avg=(256*(data_array[1]+data_array[3]+data_array[5])
                   +data_array[0]+data_array[2]+data_array[4])/3;

        data.templow = data_avg&0x00ff;
        data.temphigh = (data_avg&0xff00)>>8;
    }
    else if((addr == HUMI_BORD_ADDR)&&(cmd_id == 0x25))
    {
        quint16 temp;
        temp=256*data.temphigh+data.templow;
//        temp = (*pointer)*256+(*(pointer+1));
        data_array[0] = *(pointer+2);
        data_array[1] = *(pointer+3);
        data_array[2] = *pointer;
        data_array[3] = *(pointer+1);
        qDebug()<<"temp_read = "<<data_array[2]<<" ,"<<data_array[3] ;
//        data_avg=(256*data_array[1]+data_array[0])/25.6;
        if(data_array[1]<4.2)
            data_array[1]=4.2;
        data_avg=2.1*(256*data_array[1]+data_array[0]-4.2*256)/25.6-(temp-100)/50;
        data.humilow = data_avg&0x00ff;
        data.humihigh =  (data_avg&0xff00)>>8;
    }

    return data;

}

void Actor::setTempLimit(int temp_high,int temp_low)
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff("0");

    tempcmdbuff[0] = TEMP_BORD_ADDR;
    tempcmdbuff[1] = 0xAB;
    tempcmdbuff[2] = temp_low;
    tempcmdbuff[3] = temp_high;

    crcresult = CRC16(tempcmdbuff,4);
    crclow = crcresult & 0x00ff;
    crchigh = (crcresult&0xff00)>>8;

    writeBuff.resize(6);
    writeBuff[0] = tempcmdbuff[0];
    writeBuff[1] = tempcmdbuff[1];
    writeBuff[2] = tempcmdbuff[2];
    writeBuff[3] = tempcmdbuff[3];
    writeBuff[4] = crclow;
    writeBuff[5] = crchigh;

    mutex.lock();
    com_actor->flush();
    com_actor->write(writeBuff.data(),writeBuff.size());
    mutex.unlock();

    qDebug() << "setTempLimit = " << writeBuff.toHex();
    writeBuff.clear();


    this->msleep(50);

    readSensorData();
}

void Actor::checkTemp()
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff("0");

    tempcmdbuff[0] = TEMP_BORD_ADDR;
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

    qDebug() << "writeTempcmd = " << writeBuff.toHex();
    writeBuff.clear();

    this->msleep(50);
    readSensorData();

}

void Actor::getTempLimit()
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff("0");

    tempcmdbuff[0] = TEMP_BORD_ADDR;
    tempcmdbuff[1] = 0x0b;

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

    qDebug() << "writeTempcmd = " << writeBuff.toHex();
    writeBuff.clear();

    this->msleep(50);
    readSensorData();
}


void Actor::checkHumi()
{
    qint16 crcresult = 0;
    qint8   crchigh = 0;
    qint8   crclow = 0;
    QByteArray tempcmdbuff("0");

    tempcmdbuff[0] = HUMI_BORD_ADDR;
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

    qDebug() << "writeHumicmd = " << writeBuff.toHex();
    writeBuff.clear();

    this->msleep(50);
    readSensorData();

}
