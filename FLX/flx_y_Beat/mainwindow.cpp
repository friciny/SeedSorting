#include "mainwindow.h"

QMutex mutex;
QMutex syncmutex;
RealtimeData rtdata;
RealtimeCmd rtcmd;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mycom_485 = new Posix_QextSerialPort(ACTOR_PORT,QextSerialBase::Polling);
    mycom_screen = new Posix_QextSerialPort(SCREEN_PORT,QextSerialBase::Polling);

    if(mycom_485 ->open(QIODevice::ReadWrite))
    {
        qDebug() << "mycom_485 open.\n";
    }else
        qDebug() << "mycom_485 open faild .\n";

    if(mycom_screen ->open(QIODevice::ReadWrite))
    {
        qDebug() << "mycom_screen open.\n";
    }else
        qDebug() << "mycom_screen open faild .\n";

    mycom_485->setBaudRate(BAUDRATE);
    mycom_485->setDataBits(DATABITS);
    mycom_485->setParity(SETPARITY);
    mycom_485->setStopBits(STOPBITS);
    mycom_485->setFlowControl(FLOWCTL);
    mycom_485->setTimeout(TIME_OUT);

    mycom_screen->setBaudRate(BAUDRATE);
    mycom_screen->setDataBits(DATABITS);
    mycom_screen->setParity(SETPARITY);
    mycom_screen->setStopBits(STOPBITS);
    mycom_screen->setFlowControl(FLOWCTL);
    mycom_screen->setTimeout(TIME_OUT);


    myScreen = new Screen(mycom_screen);
    myActor = new Actor(mycom_485);
    myWindInv = new Inverter(mycom_485,WIND_INV_ADDR);
    myShakeInv = new Inverter(mycom_485,SHAKE_INV_ADDR);

    myScreen->start();
    myActor->start();
    myWindInv->start();
    myShakeInv->start();

}



MainWindow::~MainWindow()
{
    delete myScreen;
    delete myActor;
    delete myWindInv;
    delete myShakeInv;
}
