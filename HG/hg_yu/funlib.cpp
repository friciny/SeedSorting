#include "funlib.h"

qint16 char2int16(char *p)
{
    union change
    {
        qint16 d;
        char dat[2];
    }r1;

    r1.dat[0] = *(p+1);
    r1.dat[1] =  *p;
    return r1.d;
}

quint16 CRC16(const char *dat, quint16 len)//CRC16校验
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

quint8 checksum(const QByteArray &buff)
{
    quint8 result = buff[0];
    for( int i = 1;i <11;i++)
    {
        result = result^buff[i];
    }
    return result;
}
