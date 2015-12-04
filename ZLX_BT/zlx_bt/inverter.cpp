#include "inverter.h"

extern QMutex mutex;
extern QMutex syncmutex;
extern RealtimeData rtdata;
extern RealtimeCmd rtcmd;

Inverter::Inverter(QObject *parent) :
    QThread(parent)
{
}

Inverter::Inverter(Posix_QextSerialPort *com,int addr)
{
    this->com_inverter = com;
    this->InverterAddr = addr;
    this->init();
}
Inverter::~Inverter()
{
    this->stop();
}


void Inverter::run()
{
    while(1)
    {
       syncmutex.lock();

        if(rtcmd.stopflag)
        {
            this->stop();
        }
        else
        {
            this->stop();
            getInverterFreq();
            readInverterData();
            setInverterFreq();
        }

       syncmutex.unlock();
        this->msleep(150);
    }

}


void Inverter::init()
{
    writeInverterCmd(PZD1_H_WRITE ,PZD1_L_STOP, PZD2_H_STOP ,PZD2_L_STOP);
}

void Inverter::stop()
{
    writeInverterCmd(PZD1_H_WRITE ,PZD1_L_STOP, PZD2_H_STOP ,PZD2_L_STOP);
}

void Inverter::readInverterData()
{
 //   this->com_inverter->setParity(PAR_EVEN);
    mutex.lock();
    readBuff = com_inverter->read(14);
    mutex.unlock();

    if(readBuff.isEmpty())
        qDebug() << "InverterData is Empty";
    else
    {
        qDebug() << "InverterData is " << readBuff.toHex();
        //resolve the freqency of Inverter
        rtdata = resolveInvData(readBuff);

        qDebug()<<"rtdata.wind_freq = "<<rtdata.wind_freq;
        qDebug()<<"rtdata.shake_freq = "<<rtdata.shake_freq;
    }

}

void Inverter::writeInverterCmd(quint8 PZD1_high ,quint8 PZD1_low,quint8 PZD2_high,quint8 PZD2_low)
{
    writeBuff.resize(12);
    writeBuff.clear();
    writeBuff[0] = 0x02;
    writeBuff[1] = 0x0a;
    writeBuff[2] = InverterAddr;
    writeBuff[3] = 0x00;
    writeBuff[4] = 0x00;
    writeBuff[5] = 0x00;
    writeBuff[6] = 0x00;
    writeBuff[7] = PZD1_high;
    writeBuff[8] = PZD1_low;
    writeBuff[9] = PZD2_high;
    writeBuff[10] = PZD2_low;
    writeBuff[11] = checksum(writeBuff);

    mutex.lock();
    this->com_inverter->setParity(PAR_EVEN);
    com_inverter->flush();
    com_inverter->write(writeBuff.data(),12);

    this->com_inverter->setParity(PAR_NONE);
    mutex.unlock();

    qDebug() << "writeInverterCMD is " << writeBuff.toHex();

    writeBuff.clear();
    this->msleep(50);

}

void Inverter::writeInverterFreq(qint8 freq)
{
    if((freq <= 50)&&(freq >=5))
    {
        quint8 freq_high ,freq_low;
        freq_low = (freq/50.0*16384);
        freq_high = (qint16)(freq/50.0*16384)>>8;
        writeInverterCmd(PZD1_H_WRITE ,PZD1_L_RUN ,freq_high ,freq_low);
    }
}

void Inverter::getInverterFreq()
{
    writeInverterCmd(PZD1_H_READ , PZD1_L_RUN , PZD2_H_STOP ,PZD2_L_STOP);
}

void Inverter::setInverterFreq()
{
    if(InverterAddr == WIND_INV_ADDR)
    {
        if(rtcmd.wind_freq == rtdata.wind_freq)
        {

        }
        else if (rtcmd.wind_freq == 0)
        {
            this->stop();
        }
        else
        {
            qDebug()<<"set wind_freq = "<<rtcmd.wind_freq;
            this->writeInverterFreq(rtcmd.wind_freq);
        }
    }
    else if(InverterAddr == SHAKE_INV_ADDR)
    {
        if(rtcmd.shake_freq == rtdata.shake_freq)
        {

        }
        else if (rtcmd.shake_freq == 0)
        {
            this->stop();
        }
        else
        {
            qDebug()<<"set shake_freq = "<<rtcmd.shake_freq;
            this->writeInverterFreq(rtcmd.shake_freq);
        }
    }
}

RealtimeData Inverter::resolveInvData(QByteArray &buff)
{
    RealtimeData data;
    char *pointer;
    quint32 freq;
    qint8 invflag;
    qint8 addr;
    quint8 freq_high ,freq_low;

    data = rtdata; //must to do

    pointer = buff.data();
    invflag = *pointer;
    //qDebug() << "inflag = " <<invflag;

    if(invflag == 0x02)
    {
        pointer += 2;
        addr = *pointer;
        qDebug() <<"addr" <<addr;

        pointer += 7;
        freq_high = *pointer++;
        freq_low = *pointer;

        freq = freq_high;
        freq = freq<<8;
        freq += freq_low;

        freq = (quint32)(freq*5)/1630;

        if(addr == WIND_INV_ADDR)
        {
            data.wind_freq = freq;
        }
        else if( addr == SHAKE_INV_ADDR)
        {
            data.shake_freq = freq;
        }
    }

    pointer = NULL;
    return data;
}
