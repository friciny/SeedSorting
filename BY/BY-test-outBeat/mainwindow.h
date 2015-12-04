#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "screen.h"
#include "sensor.h"
#include "timecount.h"
#include "actor.h"
#include "global.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Posix_QextSerialPort *mycom;
    Posix_QextSerialPort *mycom_screen;
    Posix_QextSerialPort *mycom_actor;
    void serialInit();

private:
    Screen *myscreen;
    Sensor *mysensor;
    Timecount *mytime;
    //Actor *mytick;
    QByteArray buffer;
    qint16 char2int16(char*);
};

#endif // MAINWINDOW_H
