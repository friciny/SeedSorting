#ifndef SENSOR_H
#define SENSOR_H

#include "global.h"
#include  "screen.h"

#define CMD1_READ_SENSOR_DATA    0x0D   // 附加命令1
#define CMD2_READ_SENSOR_DATA    0x0F   // 附加命令2




class Sensor : public QThread
{
public:
    Sensor(Posix_QextSerialPort*);
    ~Sensor();
    void getSensorData();
    bool readComRawSensorData();
    void getLiquidData();
    bool readComLiquidSensorData();
    qint16 char2Int16(char*);
    int str2Hex(QString);
    char ConvertHexChar(char);
    static RealtimeData rtdata;
    static Controlcount tickdata;
    void cheek02();
    void cheek03();
    void  cheek01();

protected:
    void run();

private:
    bool readsensorfail;
    //SensorData sensordata;
    //RealtimeData rtdata;
    QByteArray rawdata;
    QByteArray readdata;
    Posix_QextSerialPort *mycom;
    qint8 switch1;
    qint8 switch2;
    quint16 CRC16(const char *, quint16);                    // crc校验
};

#endif // SENSOR_H
