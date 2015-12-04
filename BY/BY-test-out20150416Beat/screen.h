#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "global.h"
#include "timecount.h"
#include "sensor.h"
#define CMD_READ_SCREEN2_DATA    0x02
#define CMD_READ_SCREEN3_DATA    0x03
#define CMD_READ_SCREEN4_DATA    0x04
#define CMD_READ_SCREEN5_DATA    0x05
#define CMD_READ_SCREEN6_DATA    0x06
#define CMD_READ_SCREEN7_DATA    0x07

class Screen : public QThread
{
public:
    Screen(Posix_QextSerialPort*);
    ~Screen();
    void writeScreenData();
    qint16 char2Int16(char*);
    bool readComRawScreenData();

    static ManDataout modata;
    static AutoDataout aodata;


private:
    bool readsensorfail;
    RealtimeData rtdata;
    Screen3Data sndata;
    Washtube wtdata;
    HoldingOn hodata;
    QByteArray rawdata;
    QByteArray readdata;
    ManDatain midata;
    Posix_QextSerialPort *mycom_screen;


protected:
    void run();
};


#endif // SCREEN_H
