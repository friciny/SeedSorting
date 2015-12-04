#include "timecount.h"
#include "global.h"
#include <iostream>
#include <QTimer>

extern RealtimeCount rtcount;

Timecount::Timecount()
{
    rtcount.CountSecond = 0;
    rtcount. CountMinute = 0;
    rtcount.CountHour = 0;
}

void Timecount::run()
{
    QTimer *m_timer=new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(count()),Qt::DirectConnection);
    m_timer->start(1000);
    exec();
}

Timecount::~Timecount()
{
}

void Timecount::count()
{
    rtcount.CountSecond++;
    if(rtcount.CountSecond>=60)
    {
        rtcount.CountMinute++;
        rtcount.CountSecond = 0;
    }
    if(rtcount.CountMinute>=60)
    {
       rtcount. CountHour++;
       rtcount. CountMinute = 0;
    }

    qDebug()<<"CountSecond="<<rtcount.CountSecond;
    qDebug()<<"CountMunite="<<rtcount.CountMinute;
    qDebug()<<"CountHour="<<rtcount.CountHour;
}


