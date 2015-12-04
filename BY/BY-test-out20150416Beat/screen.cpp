#include "screen.h"
#include "global.h"
#include <iostream>
#include "actor.h"

extern Timecountout Timecount::tidata;
extern RealtimeData Sensor::rtdata;
extern Controlcount Sensor::tickdata;
extern QMutex syncmutex;
qint8 lefttemp=0;
qint8 righttemp=0;
int screen_id= 0x00;
extern qint16 addr;
bool liquidfAddflag=false;
bool washcountflag=false;
bool leftflag=false;
bool rightflag=false;
bool weightflag1=false;
bool AutoCountflag=false;
bool countflag=false;


Screen::Screen(Posix_QextSerialPort *com) : QThread()
{
    mycom_screen = com;//设定通信端口
     Timecount::tidata.counts=0;
}

void Screen::run()
{
    while(1)
    {  
        syncmutex.lock();
        qDebug() << "Screen::run() is running.";
        readComRawScreenData();
        writeScreenData();
        syncmutex.unlock();
        this->msleep(100);
    }

}

Screen::~Screen()
{
}


bool Screen::readComRawScreenData()//从屏幕读取数据
{
    QMutex mutex;
    char *p;
    bool return_value;
    qint8 temp = 0;
    mutex.lock();
    readdata = mycom_screen->read(16);
    mutex.unlock();

    qDebug()<<"blanking1_reach:"<<Sensor::rtdata.blanking1_reach;
    if(readdata.isEmpty())
    {
        qDebug() << "Screen data empty.";
        return_value = false;
    }
    else
    {
        qDebug() << "Screen_data acquired.";
        qDebug() << "Screen_data length : " << readdata.length();
        qDebug() << "Screen_data : " << readdata.data();
        qDebug() << "Screen_data.toHex : " << readdata.toHex();
    }

    p = readdata.data();
    screen_id = *p;
    qDebug() << "Screen_id="<<screen_id;

    if(screen_id == CMD_READ_SCREEN3_DATA)//参数设置界面
    {
        temp=*p;
        sndata.add1=temp;//地址
        p++;
        temp = *p;
        sndata.data2 = temp;
        p++;
        temp = *p;
        sndata.data3 = temp;
        p++;
        temp = *p;
        sndata.data4 = temp;
        p++;
        temp = *p;
        sndata.data5 = temp;
        p++;
        temp = *p;
        sndata.data6 = temp;
        p++;
        temp = *p;
        sndata.data7 = temp;
        p++;
        temp = *p;
        sndata.data8 = temp;
        p++;
        temp = *p;
        sndata.data9 = temp;
        p++;
        temp = *p;
        sndata.data10 = temp;
        p++;
        temp = *p;
        sndata.data11 = temp;
        p++;
        temp = *p;
        sndata.data12 = temp;
        p++;
        temp = *p;
        sndata.data13 = temp;
        p++;
        temp = *p;
        sndata.data14 = temp;
        p++;
        temp = *p;
        sndata.data15 = temp;
        //p++;
        //temp = *p;
        //sndata.check2 = temp;

        Sensor::rtdata.leftliquidbegintime=sndata.data2*60+sndata.data3;//保存设定的时间数据
        Sensor::rtdata.rightliquidbegintime=sndata.data4*60+sndata.data5;
        Sensor::rtdata.powderbegintime=sndata.data6*60+sndata.data7;
        Sensor::rtdata.powderendtime=sndata.data8*60+sndata.data9;
        Sensor::rtdata.batchtime=sndata.data10*60+sndata.data11;
        Sensor::rtdata.mixtime=sndata.data12*60+sndata.data13;
        Sensor::rtdata.batchcounts=sndata.data14;
        Sensor::rtdata.weighstatus=sndata.data15*10;//保存设定的重量数据
        //Actor::tickdata.weighstep=1;
        qDebug()<<"leftliquidtime="<<Sensor::rtdata.leftliquidbegintime;
        qDebug()<<"rightquidtime="<<Sensor::rtdata.rightliquidbegintime;
        qDebug()<<"powderbegin="<<Sensor::rtdata.powderbegintime;
        qDebug()<<"powderend="<<Sensor::rtdata.powderendtime;


        return_value = true;
    }

    else if(screen_id == CMD_READ_SCREEN4_DATA)//加液管清洗界面
    {
        temp=*p;
        wtdata.add1=temp;//地址
        p++;
        temp = *p;
        wtdata.lefttube = temp;//开启左加液
        p++;
        temp = *p;
        wtdata.righttube = temp;//开启右加液
        return_value = true;
    }

    else if(screen_id == CMD_READ_SCREEN5_DATA)//手动模式界面
    {
        Sensor:: tickdata.leftpumpstep = 1;    //3.25add
        Sensor:: tickdata.powderstirstep=1;   //3.27add

        temp=*p;
        midata.add1=temp;//地址
        p++;
        temp = *p;
        midata.motor = temp;
        p++;
        temp = *p;
        midata.wind = temp;
        p++;
        temp = *p;
        midata.weigh = temp;
        p++;
        temp = *p;
        midata.blanking = temp;
        p++;
        temp = *p;
        midata.leftliquid = temp;
        p++;
        temp = *p;
        midata.rightliquid = temp;
        p++;
        temp = *p;
        midata.powder = temp;
        p++;
        temp = *p;
        midata.discharging = temp;
        //p++;
        //temp = *p;
        //midata.check2 = temp;
        return_value = true;
    }

    else if(screen_id == CMD_READ_SCREEN6_DATA)//自动模式界面（功能待完善）
    {
        temp=*p;
        aodata.addin=temp;//地址
        p++;
        midata.motor=*p;

        Sensor:: tickdata.leftpumpstep = 1;    //4.8add
        Sensor:: tickdata.powderstirstep=1;   //4.8add

        return_value = true;
    }

    else if(screen_id == CMD_READ_SCREEN7_DATA)//自动模式暂停界面（功能待完善）
    {
        temp=*p;
        hodata.add1=temp;
        p++;
        temp=*p;
        hodata.command1=temp;

        return_value=true;
    }

    else if(screen_id == CMD_READ_SCREEN2_DATA)//返回主界面，各状态复位
    {
        liquidfAddflag=false; //0330
        modata.add1='0';
        midata.add1=0;
        midata.weigh=0;
        midata.blanking=0;
        midata.leftliquid=0;
        midata.rightliquid=0;
        midata.powder=0;
        midata.discharging=0;
        modata.weigh='0';
        modata.blanking='0';
        modata.leftliquid='0';
        modata.rightliquid='0';
        modata.powder='0';
        modata.discharging='0';
        modata.leftcount='0';
        modata.rightcount='0';
        Timecount::tidata.counts=0;
        Sensor::rtdata.weight_reach=false;
        Sensor::rtdata.blanking1_reach=false;
        washcountflag=false;
        weightflag1=false;

        Sensor::rtdata.weighbegin=0;

        Sensor::rtdata.liquidadd =0;          //0327
        Sensor::rtdata.leftliquidbegin=5;
        Sensor::rtdata.rightliquidbegin=5;

        Sensor::tickdata.lefttubebegin=5;  //0327
        Sensor::tickdata.righttubebegin=5 ;  //0327
        wtdata.add1=0; //0327
        lefttemp=0;
        righttemp=0;
        AutoCountflag=false;
        countflag=false;

        Sensor:: tickdata.leftpumpstep=5;    //3.25add
        Sensor:: tickdata.rightpumpstep=5;    //3.25add
        Sensor:: tickdata.powderstirstep=5;    //3.25add

        Sensor::tickdata.actorreset=1;
        Sensor::tickdata.motorstep=0;
        Sensor::tickdata.windstep=0;
        Sensor::tickdata.blankingstep1=0;
        Sensor::tickdata.powderstep=0;
        Sensor::tickdata.dischargingstep1=0;

        Screen::modata.mixtimemin='0';
        Screen::modata.mixtimesechigh='0';
        Screen::modata.mixtimeseclow='0';

        Screen::aodata.addin=0;
        Screen:: aodata.motor='0';
        Screen::aodata.wind='0';
        Screen:: aodata.weigh='0';
        Screen:: aodata.blanking='0';
        Screen::aodata.leftliquid='0';
        Screen::aodata.rightliquid='0';
        Screen:: aodata.powder='0';
        Screen:: aodata.discharging='0';
        Screen::rtdata.batchcounts1=0;
        Screen::rtdata.sumweigherror=0;
        Sensor::rtdata.discharging_reach=false;
        return_value = true;
    }
    else
        return_value=false;



    qDebug() << "screenman_add = " << midata.add1;
    qDebug() << "screenauto_add = " << aodata.addin;
    qDebug()<< "wind="<< modata.wind;
    qDebug()<< "motor="<< modata.motor;

    p = NULL;
    return return_value;
}


qint16 Screen::char2Int16(char *p)//两字节的char型转换成16进制数
{

    union change{
        qint16 d;
        char dat[2];
    }r1;

    r1.dat[0] = *(p+1);
    r1.dat[1] = *p;
    return r1.d;
}

void Screen::writeScreenData()//向屏幕发送命令或数据
{
    QMutex mutex;

    QByteArray buff;

    if(wtdata.add1==4)//清洗加液管命令
    {
        Sensor:: tickdata.leftpumpstep = 1; //0327

        if((liquidfAddflag==false)&&(Sensor::rtdata.liquidadd!=3))
        {
            Sensor::rtdata.liquidadd = 1;   //liquid add
        }

        if(wtdata.lefttube==1)
        {
            liquidfAddflag=true;
            washcountflag=false;
            modata.leftcount='0'+(Sensor::rtdata.leftcount+lefttemp);

            if(Sensor::tickdata.lefttubebegin==4)
            {
                Sensor::tickdata.lefttubebegin=2;
            }
            else
                Sensor::tickdata.lefttubebegin=1;
        }

        if(wtdata.righttube==1) // rightwash 0327
        {
            liquidfAddflag=true;
            washcountflag=false;
            modata.rightcount='0'+(Sensor::rtdata.rightcount+righttemp);

            if(Sensor::tickdata.righttubebegin==4)
            {
                Sensor::tickdata.righttubebegin=2;
            }
            else
                Sensor::tickdata.righttubebegin=1;
        }

        if((wtdata.lefttube==3)&&(wtdata.righttube==3)) //wash_stop 0327
        {
            Sensor::rtdata.liquidadd = 1;
            if(washcountflag==false)
            {
                lefttemp+=Sensor::rtdata.leftcount;
                righttemp+=Sensor::rtdata.rightcount;
            }
            washcountflag=true;
            wtdata.lefttube=0;
            wtdata.righttube=0;
            Sensor::tickdata.lefttubebegin=3;
            Sensor::tickdata.righttubebegin=3 ;
            liquidfAddflag=false;

            modata.leftcount='0'+lefttemp;
            modata.rightcount='0'+righttemp;
        }
        //向屏幕发送次数
        buff.resize(3);
        buff[0] ='9';
        buff[1] =modata.leftcount;
        buff[2] =modata.rightcount;

        qDebug() << "lefttemp="<<lefttemp;
        qDebug() << "righttemp="<<righttemp;
        qDebug()<<"modata.leftconut="<<modata.leftcount;
        qDebug()<<"modata.rightcount="<<modata.rightcount;

        qDebug() << "Buff_toHex2 : " << buff.toHex();

        mutex.lock();
        //发送命令前先清空接收缓存
        mycom_screen->flush();
        mycom_screen->write(buff.data(),buff.size());
        mutex.unlock();
        this->msleep(80);
    }

    if(midata.add1==5)//在手动模式下发送命令
    {
        modata.add1='5';
        if(liquidfAddflag==false)
        {
            Sensor::rtdata.liquidadd = 1;
        }
        else
            liquidfAddflag=true;

        if(midata.motor==1)//搅拌电机被用户开启
        {
            if((Sensor::tickdata.motorstep!=3)&&(Sensor::tickdata.motorstep!=1))
                Sensor::tickdata.motorstep=1;//发送开启指令
            //modata.motor='2';
        }
        else//搅拌电机被用户关闭
        {
            if((Sensor::tickdata.motorstep!=4)&&(Sensor::tickdata.motorstep!=0))
                Sensor::tickdata.motorstep=0;//发送关闭指令

        }

        if(midata.wind==1)//鼓风机被用户开启
        {
            if((Sensor::tickdata.windstep!=3)&&(Sensor::tickdata.windstep!=1))
                Sensor::tickdata.windstep=1;//发送开启指令
            //modata.motor='2';
        }
        else//鼓风机被用户关闭
        {
            if((Sensor::tickdata.windstep!=4)&&(Sensor::tickdata.windstep!=0))
                Sensor::tickdata.windstep=0;//发送关闭指令

        }

        if(midata.weigh==1)//用户开启称重步骤
        {
            qDebug()<<"rtdata.weightflag="<<rtdata.weighflag;
            if((Sensor::rtdata.weighflag==0x01)&&(weightflag1==true))
            {
                Sensor::rtdata.weight_reach=true;
                modata.weigh='2';
                Sensor::rtdata.weighbegin=3;
            }
            else if((Sensor::rtdata.weighflag==0x02)||(Sensor::rtdata.weighflag==0x03))
            {
                weightflag1=true;
            }
            else if((Sensor::rtdata.weighbegin!=2)&&(Sensor::rtdata.weight_reach==false))
            {
                Sensor::rtdata.weighbegin=1;//发送称重指令
               // Sensor::rtdata.weight_reach=true;  //调试用
                //modata.weigh='2'; //调试用
            }
        }

        if(midata.blanking==1)//用户开启下料步骤
        {
            if(Sensor::rtdata.blanking1_reach==false)
            {
                qDebug()<<"rtdata.actorstatus="<<Sensor::rtdata.actorstatus; //3.25add
                Sensor::tickdata.blankingstep1=1;
            }
            if(( Sensor:: rtdata.actorstatus&0x06) != 0)//0325 add
            {
                Sensor::rtdata.blanking1_reach=true;
                Sensor::tickdata.blankingstep1=5;
            }
            else if(Sensor::rtdata.blanking1_reach==true)
                modata.blanking='2';
        }

        if((modata.blanking=='2')&&(countflag==false))
        {
            Timecount::tidata.counts=0;//开始计时
            countflag=true;
        }

        if((midata.blanking==1)&&(liquidfAddflag==true))
        {
            Sensor::rtdata.leftliquidbegin=5;
            Sensor::rtdata.rightliquidbegin=5;
        }

        if(midata.rightliquid==1)
        {
            if(((Sensor::rtdata.liquidstatus1&0x08) ==0)&&(rightflag==true))//检测到低于下液位
            {
                rightflag=false;
                Screen::modata.rightliquid='2';//右加液完成
                midata.rightliquid=0;
                Sensor::rtdata.rightliquidbegin=3;
                qDebug() <<"right_liquit_add_finish";
            }
            else if(rightflag==false)
            {
                Sensor::rtdata.rightliquidbegin=1;//发送右加液开启指
                rightflag=true;
                modata.rightliquid='0';
            }
        }

        if(midata.leftliquid==1)
        {
            if(((Sensor::rtdata.liquidstatus1&0x02) ==0)&&(leftflag==true))//检测到低于下液位
            {
                leftflag=false;
                Screen::modata.leftliquid='2';//左加液完成
                midata.leftliquid=0;
                Sensor::rtdata.leftliquidbegin=3;
                qDebug() <<"left_liquit_add_finish";
            }
            else if(leftflag==false)
            {
                Sensor::rtdata.leftliquidbegin=1;//发送左加液开启指令
                leftflag=true;
                modata.leftliquid='0';
            }
        }


        if(midata.powder==1)//用户开启加粉步骤
        {
            if((Sensor::tickdata.powderstep!=2)&&(Sensor::tickdata.powderstep!=1))
            {
                Sensor::tickdata.powderstep=1;//发送开启指令
            }
        }
        else
            Sensor::tickdata.powderstep=0;

        if(midata.discharging==1)//用户开启出料步骤
        {
            if((Sensor::tickdata.dischargingstep1!=2)&&(Sensor::tickdata.dischargingstep1!=1))
            {
                Sensor::tickdata.dischargingstep1=1;//发送开启指令
                 countflag=false;
                 Sensor::rtdata.discharging_reach=true;
            }
        }
        else
            Sensor::tickdata.dischargingstep1=0;


        if((countflag==true)&&(Sensor::rtdata.discharging_reach==false))
       {
         Screen::modata.mixtimemin='0'+Timecount::tidata.counts/60;
         Screen::modata.mixtimesechigh='0'+(Timecount::tidata.counts%60)/10;
         Screen::modata.mixtimeseclow='0'+(Timecount::tidata.counts%60)%10;
       }

        buff.resize(14);//向屏幕发送步骤状态，重量和时间数据
        buff[0] =modata.add1;
        buff[1] ='2';
        buff[2] = '2';
        buff[3] = modata.weigh;
        buff[4] = modata.blanking;
        buff[5] = Screen::modata.leftliquid;
        buff[6] = Screen::modata.rightliquid;
        buff[7] = modata.powder;
        buff[8] = modata.discharging;
        buff[9] = '0'+(Sensor::rtdata.weight_left)/10;
        buff[10]= '0'+(Sensor::rtdata.weight_left)%10;
        buff[11]=Screen::modata.mixtimemin;
        buff[12]= Screen::modata.mixtimesechigh;
        buff[13]= Screen::modata.mixtimeseclow;

        qDebug()<<"blanking1_reach:"<<Sensor::rtdata.blanking1_reach;
        qDebug() << "Buff_toHex2 : " << buff.toHex();

        mutex.lock();
        //发送命令前先清空接收缓存
        mycom_screen->flush();
        mycom_screen->write(buff.data(),buff.size());
        mutex.unlock();
        this->msleep(50);
    }

    if((aodata.addin==6)&&(Screen::rtdata.batchcounts1<Sensor::rtdata.batchcounts))//在自动模式下发送命令
    {
        aodata.add1='6';   
        if(liquidfAddflag==false)
        {
            Sensor::rtdata.liquidadd = 1;
        }
        else
            liquidfAddflag=true;

        if((Screen::aodata.discharging=='2')&&(Screen::rtdata.discharging_reach==false)&&(AutoCountflag==true))//出料完成，记一次批次
        {
            AutoCountflag=false;
            Sensor::rtdata.blanking1_reach=false&&(Screen::rtdata.batchcounts1==Sensor::rtdata.batchcounts-1);
            Screen::aodata.weigh='0';
            Screen::aodata.blanking='0';
            Screen::aodata.leftliquid='0';
            Screen::aodata.rightliquid='0';
            Screen::aodata.powder='0';
            Screen::aodata.discharging='0';
            Screen::rtdata.batchcounts1++;
        }

        if(midata.motor==1)//搅拌电机开启
        {
            if((Sensor::tickdata.motorstep!=3)&&(Sensor::tickdata.motorstep!=1))
                Sensor::tickdata.motorstep=1;//发送开启指令
            Screen::aodata.motor='1';
            qDebug()<<"mixMotor1111111111111111111";
        }

        if(Screen::aodata.motor=='1')//鼓风机被开启
        {
            if((Sensor::tickdata.windstep!=3)&&(Sensor::tickdata.windstep!=1))
                Sensor::tickdata.windstep=1;//发送开启指令
            Screen::aodata.wind='1';
            qDebug()<<"windMotor22222222222222222222";
        }

        if((Sensor::tickdata.windstep==3)&&(Screen::rtdata.weight_reach==false))//称重开启
        {
            // Screen::aodata.weigh='2' ; //调试用
            if((Sensor::rtdata.weighflag==0x01)&&(weightflag1==true))
            {
                Sensor::rtdata.weight_reach=true;
                Screen::aodata.weigh='2';
                Sensor::rtdata.weighbegin=3;
                Screen::rtdata.sumweigherror += (Sensor::rtdata.weight_left-Sensor::rtdata.weighstatus);
            }
            else if((Sensor::rtdata.weighflag==0x02)||(Sensor::rtdata.weighflag==0x03))
            {
                weightflag1=true;
            }
            else if((Sensor::rtdata.weight_reach==false)&&(Screen::rtdata.batchcounts1<Sensor::rtdata.batchcounts-1))
            {
                Sensor::rtdata.weighbegin=1;//发送称重指令
                Screen::aodata.weigh='1';
               // Sensor::rtdata.weight_reach=true;  //调试用
            }
             qDebug()<<"weightBegin333333333333333333333333";
        }

        if((Screen::aodata.weigh=='2')&&(AutoCountflag==false)&&(Screen::rtdata.discharging_reach==false)&&(Screen::rtdata.batchcounts1<Sensor::rtdata.batchcounts))//称重完成，开始下料
        {
            qDebug()<<"343434343343434434343434343";
                if(Sensor::rtdata.blanking1_reach==false)
                {
                    qDebug()<<"rtdata.actorstatus="<<Sensor::rtdata.actorstatus; //3.25add
                    Sensor::tickdata.blankingstep1=1;
                    Screen::aodata.blanking='1';
                }
                if(( Sensor:: rtdata.actorstatus&0x06) != 0)//0325 add
                {
                    Sensor::rtdata.blanking1_reach=true;
                    Sensor::tickdata.blankingstep1=5;
                }
                else if(Sensor::rtdata.blanking1_reach==true)
                {
                    Screen:: aodata.blanking='2';
                    Timecount::tidata.counts=0;
                }
        }

        if(aodata.blanking=='2') //下料完成，开始计时
        {
            AutoCountflag=true;
            if(Timecount::tidata.counts==5)
            {
                Sensor::rtdata.weight_reach=false;
                weightflag1=false;
            }
            if((Timecount::tidata.counts>=Sensor::rtdata.leftliquidbegintime-1)&&(Timecount::tidata.counts<=Sensor::rtdata.leftliquidbegintime+1)&&(Sensor::rtdata.leftliquidbegintime!=0))
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.leftliquidbegintime="<<Sensor::rtdata.leftliquidbegintime;
                qDebug()<<"blanking444444444444444444444444";
                Sensor::rtdata.leftliquidbegin=1;
                Screen::aodata.leftliquid='1';
                leftflag=true;
            }
            if(((Sensor::rtdata.liquidstatus1&0x02) ==0)&&(leftflag==true))//检测到低于下液位
            {
                leftflag=false;
                Screen::aodata.leftliquid='2';//左加液完成
            }
            if((Timecount::tidata.counts>=Sensor::rtdata.rightliquidbegintime-1)&&(Timecount::tidata.counts<=Sensor::rtdata.rightliquidbegintime+1)&&(Sensor::rtdata.rightliquidbegintime!=0))
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.rightliquidbegintime"<<Sensor::rtdata.rightliquidbegintime;
                qDebug()<<"leftAdd5555555555555555555555555555";
                Sensor::rtdata.rightliquidbegin=1;
                Screen::aodata.rightliquid='1';
                rightflag=true;
            }
            if(((Sensor::rtdata.liquidstatus1&0x08) ==0)&&(rightflag==true))//检测到低于下液位
            {
                rightflag=false;
                Screen::aodata.rightliquid='2';//右加液完成
            }
            if((Timecount::tidata.counts>=Sensor::rtdata.powderbegintime)&&(Timecount::tidata.counts<=Sensor::rtdata.powderendtime)&&(Sensor::rtdata.powderbegintime!=0))
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.powderbegintime="<<Sensor::rtdata.powderbegintime;
                qDebug()<<"rightAdd6666666666666666666666666";
                Sensor::tickdata.powderstep=1;
                Screen::aodata.powder='1';
            }
            if(Timecount::tidata.counts>=Sensor::rtdata.powderendtime)
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.powderendtime="<<Sensor::rtdata.powderendtime;
                qDebug()<<"powderAddbegin7777777777777777777777";
                Sensor::tickdata.powderstep=0;
                Screen::aodata.powder='2';
            }
            if(Timecount::tidata.counts>=Sensor::rtdata.mixtime)
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.mixtime="<<Sensor::rtdata.mixtime;
                qDebug()<<"powderAddend8888888888888888888888";
                Sensor::tickdata.dischargingstep1=1;
                Screen::aodata.discharging='1';
                Screen::rtdata.discharging_reach=true;
            }
            if(Timecount::tidata.counts>=Sensor::rtdata.batchtime)
            {
                qDebug()<<"Timecount::tidata.counts="<<Timecount::tidata.counts;
                qDebug()<<"Screen::rtdata.batchtime="<<Sensor::rtdata.batchtime;
                qDebug()<<"dischargingBegin99999999999999999999999";
                Sensor::tickdata.dischargingstep1=0;
                Screen::aodata.discharging='2';

            }
            if(Timecount::tidata.counts>=Sensor::rtdata.batchtime+6)
            {
                Screen::rtdata.discharging_reach=false;
            }
        }

        buff.resize(14);//向屏幕发送步骤状态，重量和时间数据
        buff[0] =Screen::aodata.add1;
        buff[1] =Screen::aodata.motor;
        buff[2] = Screen::aodata.wind;
        buff[3] = Screen::aodata.weigh;
        buff[4] = Screen::aodata.blanking;
        buff[5] = Screen::aodata.leftliquid;
        buff[6] = Screen::aodata.rightliquid;
        buff[7] = Screen::aodata.powder;
        buff[8] = Screen::aodata.discharging;
        buff[9] = '0'+(Sensor::rtdata.weight_left)/10;
        buff[10]= '0'+(Sensor::rtdata.weight_left)%10;
        buff[11]= '0'+(Screen::rtdata.sumweigherror)/10 ;
        buff[12]= '0'+(Screen::rtdata.sumweigherror)%10;
        buff[13]= '0'+Screen::rtdata.batchcounts1 ;

        qDebug() << "auto_Buff_toHex2 : " << buff.toHex();

        mutex.lock();
        //发送命令前先清空接收缓存
        mycom_screen->flush();
        mycom_screen->write(buff.data(),buff.size());
        mutex.unlock();
        this->msleep(50);
    }
}
