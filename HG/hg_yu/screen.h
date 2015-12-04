#ifndef SCREEN_H
#define SCREEN_H

#include "global.h"
#define DATA_SET        1
#define MANUAL          2
#define AUTO                3
#define DATA_SEND     4

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
