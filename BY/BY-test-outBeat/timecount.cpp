#include "timecount.h"
#include "global.h"
#include <iostream>
#include <QTimer>

Timecount::Timecount()
{
    tidata.counts=0;
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
    this->tidata.counts++;
    qDebug()<<"................................"<<this->tidata.counts;
}


