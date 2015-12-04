#include "sensor.h"
#include "global.h"
#include <iostream>
#include "screen.h"

extern QMutex syncmutex;
extern AutoDataout Screen::aodata;
extern ManDataout Screen::modata;
qint8 id = 0;
qint16 addr = 0;
qint16 addr2 = 0;
quint8 temp=0;
quint8 temp2=0;

Sensor::Sensor(Posix_QextSerialPort *com) : QThread()
{
    mycom = com;//设定通信端口
}

void Sensor::run()
{
    while(1){
        syncmutex.lock();

        cheek01();
        getSensorData();
        cheek03();
        getSensorData();
        cheek02();

        qDebug() << "Sensor::run() is running.";
        syncmutex.unlock();

        this->msleep(80);
    }

}

Sensor::~Sensor()
{

}

qint16 Sensor::char2Int16(char *p)
{

    union change{
        qint16 d;
        char dat[2];
    }r1;

    r1.dat[0] = *(p+1);
    r1.dat[1] = *p;
    return r1.d;
}

int Sensor::str2Hex(QString str)
{
    int hexdata,lowhexdata;
    char lstr,hstr;
    hstr=str[0].toAscii();
    lstr=str[1].toAscii();
    hexdata = ConvertHexChar(hstr);
    lowhexdata = ConvertHexChar(lstr);

    hexdata = hexdata*16+lowhexdata;

    return hexdata;

}

char Sensor::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}


quint16 Sensor::CRC16(const char *dat, quint16 len)//校验
{
    uchar  da;
    uchar  *p;
    p =(uchar *)dat; // 转化成uchar,否则校验可能不正确
    quint16 CRCRes;
    quint32 temp;
    CRCRes = 0xffff; // 复位CRC
    while(len--)
    {
        da = (CRCRes >> 12)& 0x0000ffff;
        temp = (CRCRes << 4)& 0x0000ffff;
        temp = (temp ^ (0x1021 * (da ^(*p>>4))))& 0x0000ffff;
        da = (temp >>12)& 0x0000ffff;
        temp = (temp << 4)& 0x0000ffff;
        temp = (temp ^ (0x1021 * (da ^ (*p & 0x0f))))& 0x0000ffff;
        CRCRes=temp & 0x0000ffff;
        p ++;

    }
    return CRCRes;
}

void Sensor::cheek01() //轮巡01板
{
    QMutex mutex;
    QByteArray buff;
    QByteArray tempbuff;
    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;

    buff.resize(4);
    tempbuff.resize(2);

    tempbuff[0]=WEIGHT_ADDR2;
    tempbuff[1]=0x02;

    crcresult = CRC16(tempbuff,2);
    crclow= crcresult & 0x00ff;
    crchigh = (crcresult & 0xff00)>>8;

    buff[0]=tempbuff[0];
    buff[1]=tempbuff[1];
    buff[2]=crclow;
    buff[3]=crchigh;

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();
    mycom->flush();
    mycom->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();

    this->msleep(100);

    readComRawSensorData();
}

void Sensor::cheek02()//轮巡02板
{
    QMutex mutex;
    QByteArray buff;
    QByteArray tempbuff;
    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;

    buff.resize(4);
    tempbuff.resize(2);

    tempbuff[0]=ACTOR_ADDR;
    tempbuff[1]=0x02;

    crcresult = CRC16(tempbuff,2);

    crclow= crcresult & 0x00ff;
    crchigh = (crcresult & 0xff00)>>8;

    buff[0]=tempbuff[0];
    buff[1]=tempbuff[1];
    buff[2]=crclow;
    buff[3]=crchigh;

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();
    mycom->flush(); //发送命令前先清空接收缓存
    mycom->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();

    this->msleep(100);

    readComRawSensorData();

}

void  Sensor::cheek03()//轮巡03板
{
    QMutex mutex;
    QByteArray buff;
    QByteArray tempbuff;
    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;

    buff.resize(4);
    tempbuff.resize(2);

    tempbuff[0]=LIQUID_ADDR;
    tempbuff[1]=0x02;

    crcresult = CRC16(tempbuff,2);
    crclow= crcresult & 0x00ff;
    crchigh = (crcresult & 0xff00)>>8;

    buff[0]=tempbuff[0];
    buff[1]=tempbuff[1];
    buff[2]=crclow;
    buff[3]=crchigh;

    qDebug() << "Actor Send buff length is " << buff.length();
    qDebug() << "Actor Send buff.toHex is " << buff.toHex();

    mutex.lock();
    mycom->flush();//发送命令前先清空接收缓存
    mycom->write(buff.data(),buff.size());
    mutex.unlock();
    buff.clear();
    this->msleep(100);
    readComRawSensorData();
}

void Sensor::getSensorData()//向传感器信号采集板或执行器控制板发送命令
{
    QMutex mutex;
    //quint8 tryConnCounts = 0;
    QByteArray buff;
    //QString str="0F";
    QByteArray tempbuff;
    quint16 crcresult=0;
    quint8 crchigh=0;
    quint8 crclow=0;
    quint8 weightemp=0;
    quint8 liquidcomd=0;
    quint8 liquidtemp=0;

    if((rtdata.weighbegin>0)&&(rtdata.weighbegin<3))
    {
        switch(rtdata.weighbegin)
        {
        case 1:
            weightemp=0x01;//开启指令
            break;
        case 2:
            weightemp=0x00;//巡检指令
        }

        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=WEIGHT_ADDR2;
        tempbuff[1]=0xA9;
        tempbuff[2]=rtdata.weighstatus;
        tempbuff[3]=weightemp;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;

        qDebug() << "Sensor Send buff length is " << buff.length();
        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);

        //rtdata.weighbegin=2;
    }

    if(Sensor::tickdata.lefttubebegin==1)
    {
        liquidtemp=0x01;

        liquidcomd=0xA9;
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=LIQUID_ADDR;
        tempbuff[1]=liquidcomd;
        tempbuff[2]=0x00;
        tempbuff[3]=liquidtemp;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;
        //Sensor::tickdata.lefttubebegin=2;

        qDebug() << "Sensor Send buff length is " << buff.length();
        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }
    if(Sensor::tickdata.righttubebegin==1)
    {
        liquidtemp=0x02;

        liquidcomd=0xA9;
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=LIQUID_ADDR;
        tempbuff[1]=liquidcomd;
        tempbuff[2]=0x00;
        tempbuff[3]=liquidtemp;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;
        //Sensor::tickdata.righttubebegin=2;

        qDebug() << "Sensor Send buff length is " << buff.length();
        //
        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }
    /*    else if((Sensor::tickdata.lefttubebegin==2)||( Sensor::tickdata.righttubebegin==2 ))
    {
        liquidcomd=0x02;
        buff.resize(4);
        tempbuff.resize(2);

        tempbuff[0]=0x02;
        tempbuff[1]=liquidcomd;

        crcresult = CRC16(tempbuff,2);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=crclow;
        buff[3]=crchigh;

        Sensor::tickdata.lefttubebegin=4;
        Sensor::tickdata.righttubebegin=4;

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }
    */
    else if((Sensor::tickdata.lefttubebegin==3)&&( Sensor::tickdata.righttubebegin==3 ))
    {
        liquidcomd=0xA9;
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=LIQUID_ADDR;
        tempbuff[1]=liquidcomd;
        tempbuff[2]=0x00;
        tempbuff[3]=0x03;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;

        qDebug() << "Sensor Send buff length is " << buff.length();

        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    if(Sensor::rtdata.liquidadd == 1)    //补液0327add
    {
        liquidcomd=0xA9;
        liquidtemp=0xfe;

        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=LIQUID_ADDR;
        tempbuff[1]=liquidcomd;
        tempbuff[2]=liquidtemp;
        tempbuff[3]=0x00;

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;

        qDebug() << "Sensor Send buff length is " << buff.length();

        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    if((rtdata.leftliquidbegin<4)||(rtdata.rightliquidbegin<4))//加液
    {
        if(rtdata.leftliquidbegin<4)
        {
            switch(rtdata.leftliquidbegin)
            {
            case 1://开启指令
            {
                liquidcomd=0xA9;
                liquidtemp=0x03;

                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=LIQUID_ADDR;
                tempbuff[1]=liquidcomd;
                tempbuff[2]=liquidtemp;
                tempbuff[3]=0x00;
                //buff=QByteArray::fromHex("F");
                // buff=QByteArray::fromHex("010A0B");

                crcresult = CRC16(tempbuff,4);

                crclow= crcresult & 0x00ff;
                crchigh = (crcresult & 0xff00)>>8;

                buff[0]=tempbuff[0];
                buff[1]=tempbuff[1];
                buff[2]=tempbuff[2];
                buff[3]=tempbuff[3];
                buff[4]=crclow;
                buff[5]=crchigh;
                rtdata.leftliquidbegin=2;
                break;
            }
            case 2://巡检指令
            {/*
                liquidcomd=0x02;
                buff.resize(4);
                tempbuff.resize(2);

                tempbuff[0]=0x03;
                tempbuff[1]=liquidcomd;

                crcresult = CRC16(tempbuff,2);

                crclow= crcresult & 0x00ff;
                crchigh = (crcresult & 0xff00)>>8;

                buff[0]=tempbuff[0];
                buff[1]=tempbuff[1];
                buff[2]=crclow;
                buff[3]=crchigh;
                rtdata.leftliquidbegin=2; */
                break;
            }
            case 3://关闭指令
            {
                liquidcomd=0xA9;
                liquidtemp=0x04;

                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=LIQUID_ADDR;
                tempbuff[1]=liquidcomd;
                tempbuff[2]=liquidtemp;
                tempbuff[3]=0x00;
                //buff=QByteArray::fromHex("F");
                // buff=QByteArray::fromHex("010A0B");

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
            }
            qDebug() << "Sensor Send buff length is " << buff.length();

            qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

            mutex.lock();

            //发送命令前先清空接收缓存
            mycom->flush();
            mycom->write(buff.data(),buff.size());
            mutex.unlock();
            buff.clear();
            this->msleep(30);
        }

        if(rtdata.rightliquidbegin<4)
        {
            switch(rtdata.rightliquidbegin)
            {
            case 1://开启指令
            {
                liquidcomd=0xA9;
                liquidtemp=0x07;

                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=LIQUID_ADDR;
                tempbuff[1]=liquidcomd;
                tempbuff[2]=liquidtemp;
                tempbuff[3]=0x00;
                //buff=QByteArray::fromHex("F");
                // buff=QByteArray::fromHex("010A0B");
                crcresult = CRC16(tempbuff,4);

                crclow= crcresult & 0x00ff;
                crchigh = (crcresult & 0xff00)>>8;

                buff[0]=tempbuff[0];
                buff[1]=tempbuff[1];
                buff[2]=tempbuff[2];
                buff[3]=tempbuff[3];
                buff[4]=crclow;
                buff[5]=crchigh;
                rtdata.rightliquidbegin=2;
                break;
            }
            case 2://巡检指令
            {
                /*        liquidcomd=0x02;
                buff.resize(4);
                tempbuff.resize(2);

                tempbuff[0]=0x03;
                tempbuff[1]=liquidcomd;

                crcresult = CRC16(tempbuff,2);

                crclow= crcresult & 0x00ff;
                crchigh = (crcresult & 0xff00)>>8;

                buff[0]=tempbuff[0];
                buff[1]=tempbuff[1];
                buff[2]=crclow;
                buff[3]=crchigh;
                rtdata.rightliquidbegin=2; */
                break;
            }
            case 3://关闭
            {
                //                liquidcomd=0xA9;
                //                liquidtemp=0x08;

                //                buff.resize(6);
                //                tempbuff.resize(4);

                //                tempbuff[0]=LIQUID_ADDR;
                //                tempbuff[1]=liquidcomd;
                //                tempbuff[2]=liquidtemp;
                //                tempbuff[3]=0x00;
                //                //buff=QByteArray::fromHex("F");
                //                // buff=QByteArray::fromHex("010A0B");

                //                crcresult = CRC16(tempbuff,4);

                //                crclow= crcresult & 0x00ff;
                //                crchigh = (crcresult & 0xff00)>>8;

                //                buff[0]=tempbuff[0];
                //                buff[1]=tempbuff[1];
                //                buff[2]=tempbuff[2];
                //                buff[3]=tempbuff[3];
                //                buff[4]=crclow;
                //                buff[5]=crchigh;
                break;
            }
            }
            qDebug() << "Sensor Send buff length is " << buff.length();
            //
            qDebug() << "Sensor Send buff.toHex is " << buff.toHex();
            mutex.lock();
            //发送命令前先清空接收缓存
            mycom->flush();
            mycom->write(buff.data(),buff.size());
            mutex.unlock();
            buff.clear();
            this->msleep(30);
        }
    }
    /* else if((rtdata.leftliquidbegin>4)&&(rtdata.rightliquidbegin>4))//关闭指令
    {
        liquidcomd=0xA9;
        liquidtemp=0x00;

        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=0x02;
        tempbuff[1]=liquidcomd;
        tempbuff[2]=liquidtemp;
        tempbuff[3]=0x00;
        //buff=QByteArray::fromHex("F");
       // buff=QByteArray::fromHex("010A0B");

        crcresult = CRC16(tempbuff,4);

        crclow= crcresult & 0x00ff;
        crchigh = (crcresult & 0xff00)>>8;

        buff[0]=tempbuff[0];
        buff[1]=tempbuff[1];
        buff[2]=tempbuff[2];
        buff[3]=tempbuff[3];
        buff[4]=crclow;
        buff[5]=crchigh;


        qDebug() << "Sensor Send buff length is " << buff.length();

        qDebug() << "Sensor Send buff.toHex is " << buff.toHex();

        rtdata.leftliquidbegin=3;
        rtdata.rightliquidbegin=3;

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }*/

    if((tickdata.powderstirstep<3)&&(Sensor::rtdata.blanking1_reach==false))//粉搅拌电机开启
    {

        temp=temp|0x10;
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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
        //qDebug() << "Actor Send buff is " << buff.data();
        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() <<  "powder_motor run";

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);

    }

    if((tickdata.rightpumpstep<3)&&(Sensor::rtdata.blanking1_reach==false))//右泵开启
    {
        temp=temp|0x20;
        tickdata.rightpumpstep=3;


        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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
        //qDebug() << "Actor Send buff is " << buff.data();
        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() <<  "right_pump_open";

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);

    }


    if((tickdata.leftpumpstep<3)&&(Sensor::rtdata.blanking1_reach==false))//左泵开启
    {
        temp=temp|0x40;
        tickdata.rightpumpstep=1;
        tickdata.leftpumpstep=3;


        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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
        //qDebug() << "Actor Send buff is " << buff.data();
        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() <<  "left_pump_open";

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);

    }


    if(tickdata.motorstep<3)//搅拌电机指令
    {
        if( Sensor::rtdata.blanking1_reach==true)
        {
            temp=temp&0xF9;
        }
        if(tickdata.motorstep==1)//搅拌电机开启
        {
            temp2=temp2|0x01;
            tickdata.motorstep=3;
        }
        else
        {
            temp2=temp2&0xFE;
            tickdata.motorstep=5; //3.25 4->5
        }
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() <<"mix_motor";
        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);


    }

    if(tickdata.windstep<3)//鼓风机指令
    {
        if( Sensor::rtdata.blanking1_reach==true)
        {
            temp=temp&0xF9;
        }

        if(tickdata.windstep==1)//开启指令
        {
            //temp=temp|0x02;
            temp=temp|0x80;
            tickdata.windstep=3;
        }
        else//关闭指令
        {
            //temp=temp&0xFD;
            temp=temp&0x7F;
            tickdata.windstep=5; //3.25 4->5
        }

        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() <<"wind_cmd";
        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    qDebug()<<"tickdata.blankingstep1="<<tickdata.blankingstep1;
    if(tickdata.blankingstep1<3)//下料指令
    {
        if(tickdata.blankingstep1!=0)
        {
            switch(tickdata.blankingstep1)
            {
            case 1://开启指令
            {

                temp=temp|0x06;
                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=ACTOR_ADDR;
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

                //tickdata.blankingstep1=0; //3.25add

                break;
            }

                //   case 2://巡检指令
                //            {

                //                buff.resize(4);
                //                tempbuff.resize(2);

                //                tempbuff[0]=0x02;
                //                tempbuff[1]=0x02;

                //                crcresult = CRC16(tempbuff,2);

                //                crclow= crcresult & 0x00ff;
                //                crchigh = (crcresult & 0xff00)>>8;

                //                buff[0]=tempbuff[0];
                //                buff[1]=tempbuff[1];
                //                buff[2]=crclow;
                //                buff[3]=crchigh;

                //            }
            }
            tickdata.blankingstep1=5;
        }
        else//关闭指令
        {
            temp=temp&0xF9;
            buff.resize(6);
            tempbuff.resize(4);

            tempbuff[0]=ACTOR_ADDR;
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

            tickdata.blankingstep1=5;
        }

        qDebug() << "Actor Send buff length is " << buff.length();

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() << "blanking ";
        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    if((tickdata.powderstep<3)&&((Screen::modata.blanking=='2')||(Screen::aodata.blanking=='2')))//加粉指令
    {
        if(Sensor::rtdata.blanking1_reach==true)
        {
            temp=temp&0xf9;
        }
        if(tickdata.powderstep!=0)
        {
            switch(tickdata.powderstep)
            {
            case 1://开启指令
            {

                temp=(temp&0xf9)|0x08;

                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=ACTOR_ADDR;
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
                tickdata.powderstep=2; //0326

                break;
            }

            case 2://巡检指令
            {

                buff.resize(4);
                tempbuff.resize(2);

                tempbuff[0]=ACTOR_ADDR;
                tempbuff[1]=0x02;

                crcresult = CRC16(tempbuff,2);

                crclow= crcresult & 0x00ff;
                crchigh = (crcresult & 0xff00)>>8;

                buff[0]=tempbuff[0];
                buff[1]=tempbuff[1];
                buff[2]=crclow;
                buff[3]=crchigh;
                tickdata.powderstep=0;
            }
            }
            tickdata.powderstep=5;
        }
        else//关闭指令
        {
            temp=temp&0xF7; //3.26add
            buff.resize(6);
            tempbuff.resize(4);

            tempbuff[0]=ACTOR_ADDR;
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
            tickdata.powderstep=5;
        }

        qDebug() << "Actor Send buff length is " << buff.length();

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() << "powder_add ";

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }


    if((tickdata.dischargingstep1<3)&&((Screen::modata.blanking=='2')||(Screen::aodata.blanking=='2')))//出料指令
    {
        if(tickdata.dischargingstep1!=0)
        {
            switch(tickdata.dischargingstep1)
            {
            case 1://开启指令
            {

                temp=(temp&0xf9)|0x01; //3.26add
                buff.resize(6);
                tempbuff.resize(4);

                tempbuff[0]=ACTOR_ADDR;
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
                tickdata.dischargingstep1=2;  //0326
                break;
            }
            case 2://巡检指令
            {
                buff.resize(4);
                tempbuff.resize(2);

                tempbuff[0]=ACTOR_ADDR;
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
            tickdata.dischargingstep1=5;
        }
        else//关闭指令
        {
            temp=temp&0xF8; //3.26add
            buff.resize(6);
            tempbuff.resize(4);

            tempbuff[0]=ACTOR_ADDR;
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

            tickdata.dischargingstep1=5;
        }

        qDebug() << "Actor Send buff length is " << buff.length();

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();
        qDebug() << "mater_out ";

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    if(tickdata.actorreset==1)//复位指令
    {

        temp=0x00;
        temp2=0x00;
        buff.resize(6);
        tempbuff.resize(4);

        tempbuff[0]=ACTOR_ADDR;
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

        tickdata.motorstep=5;
        tickdata.windstep=5;
        rtdata.leftliquidbegin=5;  //0327
        rtdata.rightliquidbegin=5; //0327
        tickdata.blankingstep1=5;
        tickdata.dischargingstep1=0;
        tickdata.powderstep=5;
        rtdata.blanking1_reach=false;     // 0325
        rtdata.powder_reach=false;
        tickdata.actorreset=0;


        qDebug() << "Actor Send buff length is " << buff.length();

        qDebug() << "Actor Send buff.toHex is " << buff.toHex();

        mutex.lock();

        //发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data(),buff.size());
        mutex.unlock();
        buff.clear();
        this->msleep(30);
    }

    // 等待串口数据
    /*    while(!readComRawSensorData())
    {
        // 重发一遍
        mutex.lock();
        // 发送命令前先清空接收缓存
        mycom->flush();
        mycom->write(buff.data());
        mutex.unlock();
        this->msleep(30);

        //读取数据的失败次数超过设定值
        if((++tryConnCounts)>ACK_NUM)
        {
            qDebug()<< "Serial Reading Fail : ";
            ++id;
            tryConnCounts = 0;
            rtdata.weight_left = 0;
            rtdata.weight_right = 0;
            break;
         }
    }
*/
}


bool Sensor::readComRawSensorData()//获取传感器数据或执行器状态
{
    QMutex mutex;
    char *p=new char;
    bool return_value=false;
    qint16 tempr = 0;
    //quint16 crcresult;
    //quint8 crchigh=0;
    //quint8 crclow=0;
    mutex.lock();
    readdata = mycom->read(20);
    mutex.unlock();
    this->msleep(60);

    if(readdata.isEmpty())
    {
        qDebug() << "Sensor data empty.";
        return_value = false;
    }
    else
    {
        qDebug() << "Sensor data acquired.";
        qDebug() << "Sensor data length is " << readdata.length();
        qDebug() << "Sensor data is " << readdata.data();
        qDebug() << "Sensor data.toHex is " << readdata.toHex();
        // return_value = true;
    }

    p = readdata.data();
    qDebug()<<"DAta="<<*p;
    addr2 = char2Int16(p);
    p++;
    addr = char2Int16(p);

    qDebug() << "addr = " << addr;
    qDebug() << "addr2 = " << addr2;
    qDebug() << "rtdata.weight_reach="<<rtdata.weight_reach;


    if((addr==0x0124)&&(rtdata.weight_reach==false))  //获取称重板数据
    {
        p=p+5;
        tempr=*p;
        rtdata.weighflag=tempr;
        p= p-2;
        tempr=char2Int16(p);
        rtdata.weighstan=tempr;//重量数据
        rtdata.weight_left=rtdata.weighstan;

        return_value=true;
        qDebug()<<"rtdata.weightflag="<<rtdata.weighflag;
        qDebug() << "weightstan = " <<rtdata.weighstan;
        qDebug() << "weight_left = " <<rtdata.weight_left;
    }

    if(addr==0x0324)//获取加液板数据
    {
        p=p+2;
        tempr=*p;
        rtdata.liquidstatus1=tempr;
        p++;
        tempr=*p;
        rtdata.liquidstatus2=tempr;//液位状态
        p++;
        tempr=*p;
        rtdata.leftcount=tempr;
        p++;
        tempr=*p;
        rtdata.rightcount=tempr;

        return_value=true;
        qDebug() << "liquidstatus1= " <<rtdata.liquidstatus1;
        qDebug() << "liquidstatus2= " <<rtdata.liquidstatus2;
        qDebug() << "leftcount= " <<Sensor::rtdata.leftcount;
        qDebug() << "rightcount = " <<Sensor::rtdata.rightcount;

    }

    if((addr==0x0292)||(addr==0x0221))//获取执行器板数据//3.25
    {
        p=p+2;

        tempr=*p;
        rtdata.actorstatus=tempr;//执行器状态

        qDebug() << "tempr= " << tempr;
        qDebug() << "atdata.actorstatus= " << rtdata.actorstatus;

        return_value = true;
    }

    p = NULL;
    delete p;//0331
    return return_value;
}
