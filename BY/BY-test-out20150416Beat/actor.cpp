/*
#include "actor.h"
#include "global.h"
#include <iostream>

extern QMutex syncmutex;
quint8 temp=0;
quint8 temp2=0;
//qint8 id = 0;
qint16 addractor = 0;
//qint16 addr2 = 0;
//quint8 tryConnCounts = 0;

Actor::Actor(Posix_QextSerialPort *com) : QThread()
{
    mycom_actor = com;
}

void Actor::run()
{
    while(1){
        syncmutex.lock();


        readComRawActorData();

        getControlTimeData();

        qDebug() << "Actor::run() is running.";
        syncmutex.unlock();
       // if(tickdata.tryConnCounts>=999)
        //    tickdata.tryConnCounts=0;
    //    else tickdata.tryConnCounts++;
      //  qDebug() << "tryConnCounts is"<<tickdata.tryConnCounts;
        this->msleep(50);
    }

}

Actor::~Actor()
{

}

// crc16 check
quint16 Actor::CRC16(const char *dat, quint16 len)
{
    uchar  da;
    uchar  *p;
    p =(uchar *)dat; // 转化成uchar,否则校验可能不正确
    quint16 CRCRes;
    CRCRes = 0xffff; // 复位CRC
    while(len--)
    {
        da = CRCRes >> 12;
        CRCRes = CRCRes << 4;
        CRCRes = CRCRes ^ (0x1021 * (da ^(*p>>4)));
        da = CRCRes >>12;
        CRCRes = CRCRes << 4;
        CRCRes = CRCRes ^ (0x1021 * (da ^ (*p & 0x0f)));
        p ++;
    }
    return CRCRes;
}

qint16 Actor::char2Int16(char *p)
{

    union change{
        qint16 d;
        char dat[2];
    }r1;

    r1.dat[0] = *(p+1);
    r1.dat[1] = *p;
    return r1.d;
}

void Actor::getControlTimeData()
{
    QMutex mutex;
    QByteArray buff;

    QByteArray tempbuff;

    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;



    if((tickdata.motorstep!=3)&&(tickdata.motorstep!=4))
    {
    if(tickdata.motorstep==1)
    {
        temp=temp|0x01;
        tickdata.motorstep=3;
    }

    else
    {
        temp=temp&0xFE;
        tickdata.motorstep=4;
    }

    buff.resize(6);
    tempbuff.resize(4);

    tempbuff[0]=0x03;
    tempbuff[1]=0xA9;
    tempbuff[2]=temp;
    tempbuff[3]=temp2;

    crcresult = CRC16(tempbuff,4);

    crclow= crcresult & 0x00ff;
    crchigh = (crcresult & 0xff00)>>8;

    buff[0]=tempbuff[0];
    buff[1]=tempbuff[1];
    buff[2]=tempbuff[2];
    buff[3]=tempbuff[3];
    buff[4]=crclow;
    buff[5]=crchigh;

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff is " << buff.data();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();

    //发送命令前先清空接收缓存
    mycom_actor->flush();
    mycom_actor->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();
    this->msleep(30);


    }

    if((tickdata.windstep!=3)&&(tickdata.windstep!=4))
    {
    if(tickdata.windstep==1)
    {
        temp=temp|0x02;
        tickdata.windstep=3;
    }
    else
    {
        temp=temp&0xFD;
        tickdata.windstep=4;
    }

    buff.resize(6);
    tempbuff.resize(4);

    tempbuff[0]=0x03;
    tempbuff[1]=0xA9;
    tempbuff[2]=temp;
    tempbuff[3]=temp2;

    crcresult = CRC16(tempbuff,4);

    crclow= crcresult & 0x00ff;
    crchigh = (crcresult & 0xff00)>>8;

    buff[0]=tempbuff[0];
    buff[1]=tempbuff[1];
    buff[2]=tempbuff[2];
    buff[3]=tempbuff[3];
    buff[4]=crclow;
    buff[5]=crchigh;

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff is " << buff.data();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();

    //发送命令前先清空接收缓存
    mycom_actor->flush();
    mycom_actor->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();
    this->msleep(30);


    }


    if(tickdata.blankingstep1!=3)
    {
    if(tickdata.blankingstep1!=0)
     {
        switch(tickdata.blankingstep1)
        {
              case 1:
              {

                      temp=temp|0x04;

                      buff.resize(6);
                      tempbuff.resize(4);

                      tempbuff[0]=0x03;
                      tempbuff[1]=0xA9;
                      tempbuff[2]=temp;
                      tempbuff[3]=temp2;

                      crcresult = CRC16(tempbuff,4);

                      crclow= crcresult & 0x00ff;
                      crchigh = (crcresult & 0xff00)>>8;

                      buff[0]=tempbuff[0];
                      buff[1]=tempbuff[1];
                      buff[2]=tempbuff[2];
                      buff[3]=tempbuff[3];
                      buff[4]=crclow;
                      buff[5]=crchigh;

                      break;
               }

               case 2:
               {

                      buff.resize(4);
                      tempbuff.resize(2);

                      tempbuff[0]=0x03;
                      tempbuff[1]=0x02;

                      crcresult = CRC16(tempbuff,2);

                      crclow= crcresult & 0x00ff;
                      crchigh = (crcresult & 0xff00)>>8;

                      buff[0]=tempbuff[0];
                      buff[1]=tempbuff[1];
                      buff[2]=crclow;
                      buff[3]=crchigh;

               }

         }

        tickdata.blankingstep1=2;

      }
    else
    {
        temp=temp&0xFB;

        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=0x03;
        tempbuff[1]=0xA9;
        tempbuff[2]=temp;
        tempbuff[3]=temp2;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;

        tickdata.blankingstep1=3;
    }

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff is " << buff.data();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();

    //发送命令前先清空接收缓存
    mycom_actor->flush();
    mycom_actor->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();
    this->msleep(30);
    }

    if(tickdata.blankingstep2==1)
        temp=temp|0x08;
    else
        temp=temp&0xF7;

    if(tickdata.blankingstep3==1)
        temp=temp|0x10;
    else
        temp=temp&0xEF;

    if(tickdata.blankingstep4==1)
        temp=temp|0x20;
    else
        temp=temp&0xDF;

    if(tickdata.powderstep==1)
        temp=temp|0x40;
    else
        temp=temp&0xBF;

    if(tickdata.dischargingstep1==1)
        temp=temp|0x80;
    else
        temp=temp&0x7F;

    if(tickdata.dischargingstep2==1)
        temp2=temp2|0x01;
    else
        temp2=temp2&0xFE;

}


bool Actor::readComRawActorData()
{

    QMutex mutex;
    QByteArray readdata;
    QByteArray readtemp;
    char *p;
    bool return_value=false;
    qint16 tempr = 0;

    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;

    mutex.lock();

    readdata = mycom_actor->read(30);

    mutex.unlock();

    if(readdata.isEmpty())
    {
       qDebug() << "Actor data empty.";
       return_value = false;
    }

   // else
   // {
        p = readdata.data();
        p++;

        addractor = char2Int16(p);
        qDebug() << "actoraddr = " << addractor;

        if(addractor==0x0321)
        {
        p=p+2;

        tempr=*p;
        atdata.actorstatus=tempr;//加液管P4四路

        qDebug() << "tempr= " << tempr;
        qDebug() << "atdata.actorstatus= " << atdata.actorstatus;

        return_value = true;
        }
   // }

     crcresult = CRC16(readdata,7);
     crclow= crcresult & 0x00ff;
     crchigh = (crcresult & 0xff00)>>8;
     readtemp.resize(2);
     readtemp[0]=crclow;
     readtemp[1]=crchigh;


     qDebug() << "Actor data acquired.";
     qDebug() << "Actor data length is " << readdata.length();
     qDebug() << "Actor data is " << readdata.data();
     qDebug() << "Actor data.toHex is " << readdata.toHex();
     qDebug() << "Actor data crclow and crchigh are " << readtemp.toHex();

     p = NULL;
     return return_value;

 }

*/
