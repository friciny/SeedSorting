#ifndef SCREEN_H
#define SCREEN_H

#include "global.h"
#define SCREEN_RUN   5
#define SCREEN_ENTR 2
#define SCREEN_STOP 3
#define CMD_INIT          1
#define DATA_SEND      3
#define CMD_CTRL        4

class Screen : public QThread
{
    Q_OBJECT
public:
    explicit Screen(QObject *parent = 0);
    Screen(Posix_QextSerialPort*);
    void readScreenData();
    void writeScreenData();

private:
    Posix_QextSerialPort *com_screen;
    QByteArray  readBuff;
    QByteArray  writeBuff;
    RealtimeCmd resolveCmd(QByteArray &buff);

protected:
    void run();

signals:
    
public slots:

};

#endif // SCREEN_H
