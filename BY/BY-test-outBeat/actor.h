/*
#ifndef ACTOR_H
#define ACTOR_H

#include "global.h"

typedef struct Controlcount
{
    qint8 motorstep;

    qint8 windstep;

    //qint8 weighstep;
    qint8 blankingstep1;
    qint8 blankingstep2;
    qint8 blankingstep3;
    qint8 blankingstep4;
    //qint8 rightliquidover;
    qint8 powderstep;
    qint8 dischargingstep1;
    qint8 dischargingstep2;

    quint8 tryConnCounts;

    Controlcount()
    {
        motorstep=0;

        windstep=0;

       // weighstep=0;
        blankingstep1=0;
        blankingstep2=0;
        blankingstep3=0;
        blankingstep4=0;
        //rightliquidover=0;
        powderstep=0;
        dischargingstep1=0;
        dischargingstep2=0;

        tryConnCounts=0;

    }
}Controlcount;


class Actor : public QThread
{
public:
    Actor(Posix_QextSerialPort*);
    ~Actor();
   void getControlTimeData();
   bool readComRawActorData();
   static ManDataout moodata;
   static AutoDataout aoodata;
   static Timecountout tidata;
   static Controlcount tickdata;
   static RealtimeData atdata;
   qint16 char2Int16(char*);
   quint16 CRC16(const char *, quint16);                    // crc校验

protected:
    void run();

private:
    //RealtimeData rtdata;
    Screen3Data sndata;
    ManDatain midata;
    Posix_QextSerialPort *mycom_actor;

    //Timecountout tidata;
};

#endif // ACTOR_H
*/
