#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "global.h"
#include "screen.h"
#include "actor.h"
#include "inverter.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Posix_QextSerialPort *mycom_485;
    Posix_QextSerialPort *mycom_screen;

    Screen *myScreen;
    Actor *myActor;
    Inverter *myWindInv;
    Inverter *myShakeInv;


};

#endif // MAINWINDOW_H
