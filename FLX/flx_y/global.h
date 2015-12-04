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

#define WIND_INV_ADDR          0x0b
#define SHAKE_INV_ADDR        0x0c
#define CTRL_BORD_ADDR       0x03
#define SENSOR_BORD_ADDR 0x04

#define WINDMOTOR_OPEN           0x80;
#define SHAKEMOTOR_OPEN         0x40;
#define FEEDING_FORWARD           0x20;
#define FEEDING_REVERSE              0x10;
#define FEEDING_STOP                      0xCF;
#define FRONTDOOR_FORWARD   0x08;
#define FRONTDOOR_REVERSE      0x04;
#define FRONTDOOR_STOP             0xF3;
#define BACKDOOR_FORWARD     0x02;
#define BACKDOOR_REVERSE        0x01;
#define BACKDOOR_STOP                0xFC;

struct RealtimeData
{
    quint8 wind_freq;
    quint8 shake_freq;
    quint8 feeding_door;
    quint8 front_door;
    quint8 back_door;

    RealtimeData()
    {
        wind_freq = 0;
        shake_freq = 0;
        feeding_door = 0;
        front_door = 0;
        back_door = 0;
    }
};

struct RealtimeCmd
{
    quint8 wind_freq;
    quint8 shake_freq;
    quint8 feeding_door;
    quint8 front_door;
    quint8 back_door;
    quint8 feeding_door_ctrl;
    quint8 front_door_ctrl;
    quint8 back_door_ctrl;
    bool wind_motor_ctrl;
    bool shake_motor_ctrl;
    bool stopflag;

    RealtimeCmd()
    {
        wind_freq = 0;
        shake_freq = 0;
        feeding_door = 0;
        front_door = 0;
        back_door = 0;
        feeding_door_ctrl = 0;
        front_door_ctrl = 0;
        back_door_ctrl = 0;
        wind_motor_ctrl =false;
        shake_motor_ctrl = false;
        stopflag = false;
    }
};




#endif // GLOBAL_H
