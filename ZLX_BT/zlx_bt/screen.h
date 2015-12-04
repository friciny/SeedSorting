#ifndef SCREEN_H
#define SCREEN_H

#include "global.h"

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
