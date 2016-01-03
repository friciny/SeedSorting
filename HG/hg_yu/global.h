#ifndef GLOBAL_H
#define GLOBAL_H

#include <QMutex>
#include <QThread>
#include <QDebug>
#include "posix_qextserialport.h"
#include "funlib.h"

#define ACTOR_PORT      "/dev/ttySAC3"
#define SCREEN_PORT    "/dev/ttySAC2"

#define BAUDRATE     BAUD9600
#define DATABITS        DATA_8
#define SETPARITY      PAR_NONE
#define STOPBITS        STOP_1
#define FLOWCTL        FLOW_OFF
#define TIME_OUT      20

#define CTRL_BORD_ADDR       0x01
#define TEMP_BORD_ADDR     0x03
#define HUMI_BORD_ADDR     0x07

#define ACTORSTOP              0x00
#define FEEDING                    0x58
#define FEEDED                      0xd8
#define CIRCULATING          0xf8

#define TEMPCTRL_1           0x81
#define TEMPCTRL_2           0x01
#define TEMPCTRL_3           0x00

struct RealtimeData
{
    quint8 temphigh;
    quint8 templow;
    quint8 humihigh;
    quint8 humilow;

    RealtimeData()
    {
        temphigh=0;
        templow=0;
        humihigh=0;
        humilow=0;
    }
};

struct RealtimeCmd
{
    quint8 temp_limit_high;
    quint8 temp_limit_low;
    quint8 humi_limit_high;
    quint8 humi_limit_low;
    quint8 state;
    bool startcount;

    RealtimeCmd()
    {
        temp_limit_high=0;
        temp_limit_low=0;
        humi_limit_high=0;
        humi_limit_low=0;
        state=5;
    }
};

struct RealtimeCount
{
    quint32 CountSecond;
    quint32 CountMinute;
    quint32 CountHour;

    RealtimeCount()
    {
        CountSecond = 0;
        CountMinute = 0;
        CountHour = 0;
    }
};




#endif // GLOBAL_H
