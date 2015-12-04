#ifndef GLOBAL_H
#define GLOBAL_H

#include <QMutex>
#include <QThread>
#include <posix_qextserialport.h>
#include <qextserialbase.h>
#include <QDebug>
#include <QWaitCondition>

#define SENSOR_PORT "/dev/ttySAC3"
#define SCREEN_PORT "/dev/ttySAC1"
#define ACTOR_PORT "/dev/ttySAC2"

#define BAUDRATE     BAUD9600
#define DATABITS     DATA_8
#define SETPARITY    PAR_NONE
#define STOPBITS     STOP_1
#define FLOWCONTRL   FLOW_OFF
#define TIME_OUT     20       //延时，TIME_OUT是串口读写的延时
#define BLOCK_SIZE   200
#define ACK_NUM      10
#define SENSOR_DATA_REQUIRE "0D"
#define SWITCH_DATA_REQUIRE "0F"
//81 c8 00 00 00 00 01
#define WEIGHT_ADDR 0x82
#define SWITCH_ADDR 0x81
#define WEIGHT_DATA_LENGTH 6
#define SWITCH_DATA_LENGTH 19
#define WEIGHT_ADDR2 0x01
#define ACTOR_ADDR 0x02
#define LIQUID_ADDR 0x03

/*
typedef struct SensorDataStruct
{
    quint8 addr;   // 地址
    qint16 weight_left;
    qint16 weight_right;
    bool   state;
    SensorDataStruct()
    {
        addr = 0x00;
        weight_left = 0;
        weight_right = 0;
        state = true;
    }

}SensorData;
*/


typedef struct RealtimeData
{
    qint16 weight_left;
    qint16 weight_right;
    qint16 weighstan;
    qint16 actorstatus;  //3.25add
    qint16 sumweigherror;

    int weightime;
    int blankingtime;
    int leftliquidbegintime;
    int rightliquidbegintime;
    int powderbegintime;
    int powderendtime;
    int batchtime;
    int mixtime;
    int dischargingtime;

    qint8 liquidstatus1;
    qint8 liquidstatus2;
    qint8 weighbegin;
    qint8 weighflag;
    qint8 leftliquidbegin;
    qint8 rightliquidbegin;
    qint8 liquidadd;   //0327

    qint8 add1;
    qint8 out3;
    qint8 out4;
    qint8 out5;
    qint8 out6;
    qint8 out7;
    qint8 out8;
    qint8 out9;
    qint8 out10;
    qint8 out11;
    qint8 out12;
    qint8 out13;
    qint8 out14;
    qint8 out15;
    qint8 in2up;
    qint8 in2down;
    qint8 in1down;
    qint8 in1up;
    qint8 leftcount;
    qint8 rightcount;

    quint8 tryConnCounts;
    quint8 weighstatus;
    quint8 batchcounts;
    quint8 batchcounts1;
    bool weight_reach;
    bool blanking1_reach;
    bool discharging_reach;
    bool powder_reach;

    RealtimeData()
    {
        weight_left = 0;
        weight_right = 0;
        sumweigherror=0;
        weight_reach= false;
        discharging_reach=false;
        blanking1_reach=false;
        powder_reach=false;
        weighstan=0;

        tryConnCounts=0;

        weightime=0;
        blankingtime=0;
        leftliquidbegintime=0;
        rightliquidbegintime=0;
        powderbegintime=0;
        powderendtime=0;
        batchtime=0;
        mixtime=0;
        dischargingtime=0;
        weighstatus=0;
        liquidstatus1=0;
        liquidstatus2=0;
        actorstatus=0;
        leftcount=0;
        rightcount=0;
        batchcounts=0;
        batchcounts1=0;

        weighbegin=0;
        weighflag=0;
        leftliquidbegin=0;
        rightliquidbegin=0;


        add1=0;
        out3 = 1;
        out4 = 1;
        out5 = 1;
        out6 = 1;
        out7 = 1;
        out8 = 1;
        out9 = 1;
        out10 = 1;
        out11 = 1;
        out12 = 1;
        out13 = 1;
        out14 = 1;
        out15 = 1;
        in2up = 1;
        in2down = 1;
        in1down = 1;
        in1up = 1;
    }
}RealtimeData;

typedef struct Controlcount
{

    qint8 lefttubebegin;
    qint8 righttubebegin;
    qint8 tubestop;

    qint8 motorstep;

    qint8 windstep;

    qint8 leftpumpstep;
    qint8 rightpumpstep;
    qint8 powderstirstep;

    //qint8 weighstep;
    qint8 blankingstep1;
    qint8 blankingstep2;
    qint8 blankingstep3;
    qint8 blankingstep4;
    //qint8 rightliquidover;
    qint8 powderstep;
    qint8 dischargingstep1;
    qint8 dischargingstep2;

    qint8 actorreset;

    quint8 tryConnCounts;

    Controlcount()
    {
        lefttubebegin=0;
        righttubebegin=0;
        tubestop=0;

        motorstep=5;

        windstep=5;

        leftpumpstep=5;
        rightpumpstep=5;
        powderstirstep=5;

       // weighstep=0;
        blankingstep1=5;
        blankingstep2=5;
        blankingstep3=5;
        blankingstep4=5;
        //rightliquidover=0;
        powderstep=5;
        dischargingstep1=5;
        dischargingstep2=5;

        actorreset=0;

        tryConnCounts=0;

    }
}Controlcount;


typedef struct CommandData
{

}CmdData;

typedef struct Screen3Data
{
    qint8 add1;
    qint8 data2;
    qint8 data3;
    qint8 data4;
    qint8 data5;
    qint8 data6;
    qint8 data7;
    qint8 data8;
    qint8 data9;
    qint8 data10;
    qint8 data11;
    qint8 data12;
    qint8 data13;
    qint8 data14;
    qint8 data15;
     qint8 check2;

    Screen3Data()
    {
        add1=0;
        data2=0;
        data3=0;
        data4=0;
        data5=0;
        data6=0;
        data7=0;
        data8=0;
        data9=0;
        data10=0;
        data11=0;
        data12=0;
        data13=0;
        data14=0;
        data15=0;
        check2=0;

    }
}Screen3Data;

typedef struct Washtube
{
    qint8 add1;
    qint8 lefttube;
    qint8 righttube;

    Washtube()
    {
        add1=0;
        lefttube=0;
        righttube=0;
    }
}Washtube;

typedef struct ManDatain
{
    qint8 add1;

    qint8 motor;
    qint8 wind;
    qint8 weigh;
    qint8 blanking;
    qint8 leftliquid;
    qint8 rightliquid;
    qint8 powder;
    qint8 discharging;

    qint8 check2;

    ManDatain()
    {
        add1=0;

        motor=0;
        wind=0;
        weigh=0;
        blanking=0;
        leftliquid=0;
        rightliquid=0;
        powder=0;
        discharging=0;

        check2=0;
    }

}ManDatain;

typedef struct ManDataout
{
    char  add1;

    char  motor;
    char  wind;
    char  weigh;
    char  blanking;
    char  leftliquid;
    char  rightliquid;
    char  powder;
    char  discharging;
    char  weighdata;
    char  mixtimemin;
    char  mixtimesechigh;
    char  mixtimeseclow;
    char leftcount;
    char rightcount;

    char check2;

    ManDataout()
    {
        add1='0';

        motor='0';
        wind='0';
        weigh='0';
        blanking='0';
        leftliquid='0';
        rightliquid='0';
        powder='0';
        discharging='0';
        weighdata='0';
        mixtimemin='0';
        mixtimesechigh='0';
        mixtimeseclow='0';
        check2='0';
        leftcount='0';
        rightcount='0';
    }

}ManDataout;



typedef struct AutoDataout
{
    qint8 addin;
    quint8 autotimes;
    quint8 autoflag;

    char add1;
    char check2;
    char motor;
    char wind;
    char weigh;
    char blanking;
    char leftliquid;
    char rightliquid;
    char powder;
    char discharging;
    char batch;

    AutoDataout()
    {
        addin=0;

        autotimes=0;
        autoflag=0;

        add1='0';
        check2='0';
        motor='0';
        wind='0';
        weigh='0';
        blanking='0';
        leftliquid='0';
        rightliquid='0';
        powder='0';
        discharging='0';
        batch='0';
    }

}AutoDataout;


typedef struct HoldingOn
{
   qint8 add1;
   qint8 command1;
   HoldingOn()
   {
       add1=0;
       command1=0;
   }
}HoldingOn;


typedef struct Timecountout
{
    int countsbasic;
    int counts ;
    int min;
    int sec;
    int highsec;
    int lowsec;

    Timecountout()
    {
         countsbasic=0;
         counts = 0;
         min=0;
         sec=0;
         highsec=0;
         lowsec=0;
    }
}Timecountout;


#endif // GLOBAL_H
