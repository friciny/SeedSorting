#include "mainwindow.h"
#include "global.h"
#include <QMutex>

QMutex syncmutex;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mycom = new Posix_QextSerialPort(SENSOR_PORT,QextSerialBase::Polling);
    mycom_screen = new Posix_QextSerialPort(SCREEN_PORT, QextSerialBase::Polling);
    mycom_actor = new Posix_QextSerialPort(ACTOR_PORT, QextSerialBase::Polling);
    //qDebug() << "Length of char : " << sizeof(char);
    //qDebug() << "Length of qint16 : " << sizeof(qint16);
    if(mycom->open(QIODevice::ReadWrite))
    {
        qDebug() << "mycom open.\n";
    }
    else
    {
        qDebug() << "mycom open failed.";
    }

    if(mycom_screen->open(QIODevice::ReadWrite))
    {
        qDebug() << "mycom_screen open.\n";
    }
    else
    {
        qDebug() << "mycom_screen open failed.";
    }

    if(mycom_actor->open(QIODevice::ReadWrite))
    {
        qDebug() << "mycom_actor open.\n";
    }
    else
    {
        qDebug() << "mycom_actor open failed.";
    }

    // Serial setup
    mycom->setBaudRate(BAUDRATE);
    mycom->setDataBits(DATABITS);
    mycom->setParity(SETPARITY);
    mycom->setStopBits(STOPBITS);
    mycom->setFlowControl(FLOWCONTRL);
    mycom->setTimeout(TIME_OUT);

    mycom_screen->setBaudRate(BAUDRATE);
    mycom_screen->setDataBits(DATABITS);
    mycom_screen->setParity(SETPARITY);
    mycom_screen->setStopBits(STOPBITS);
    mycom_screen->setFlowControl(FLOWCONTRL);
    mycom_screen->setTimeout(TIME_OUT);

    mycom_actor->setBaudRate(BAUDRATE);
    mycom_actor->setDataBits(DATABITS);
    mycom_actor->setParity(SETPARITY);
    mycom_actor->setStopBits(STOPBITS);
    mycom_actor->setFlowControl(FLOWCONTRL);
    mycom_actor->setTimeout(TIME_OUT);
    //Serial setup finished

    mysensor = new Sensor(mycom);
    myscreen = new Screen(mycom_screen);
    mytime = new Timecount();
   // mytick=new Actor(mycom_actor);

    mysensor->start();
    myscreen->start();
    mytime->start();
    //mytick->start();
}

MainWindow::~MainWindow()
{
    delete mysensor;
    delete myscreen;
    delete mytime;
    //delete mytick;
}

/*
void MainWindow::serialInit(){
    mycom = new Posix_QextSerialPort(PORTNAME,QextSerialBase::Polling);

    if(mycom->open(QIODevice::ReadWrite)){
        qDebug("Serial opened.");
    }

    qDebug("This is the serialInit method.\n");

    mycom->setBaudRate(BAUDRATE);
    mycom->setDataBits(DATABITS);
    mycom->setParity(SETPARITY);
    mycom->setStopBits(STOPBITS);
    mycom->setFlowControl(FLOWCONTRL);
    mycom->setTimeout(TIME_OUT);

    //QByteArray buffer(SENSOR_DATA_REQUIRE);
    //buffer = QByteArray::fromHex("0D");
    char *p = buffer.data();
    qint16 temp = char2int16(p);
    qDebug() << temp;
    QByteArray buffer;
    //buffer.resize(1);

    //buffer.resize(1);
    //QString str1 = buffer.toHex();
    //qDebug() << "str is : " << str1;
    //buffer.append("13");

    buffer = QByteArray::fromHex(SENSOR_DATA_REQUIRE);
    qDebug() << buffer.data();
    qDebug() << buffer.toHex();
    //qint8 str2 = buffer.toInt();
    //qDebug() << "str2 is : " << str2;

    qDebug() << "Buffer size is : " << buffer.size();
    qDebug() << "buffer is : " << buffer;
    qDebug() << "buffer.toHex is : " << buffer.toHex();
    mycom->flush();
    //mycom->write(buffer.data(),buffer.size());
    mycom->write(buffer.toHex(),buffer.size());
}
*/
