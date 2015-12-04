#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "global.h"
#include "screen.h"
#include "actor.h"
#include "inverter.h"
#include "bluetooth.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Posix_QextSerialPort *mycom_485;
    Posix_QextSerialPort *mycom_screen;
    Posix_QextSerialPort *mycom_bluetooth;

    Screen *myScreen;
    Actor *myActor;
    Bluetooth *myBluetooth;
    Inverter *myWindInv;
    Inverter *myShakeInv;


};

#endif // MAINWINDOW_H
