#ifndef TIMECOUNT_H
#define TIMECOUNT_H

#include "global.h"
#include <QTimer>
#include <QObject>

class Timecount : public QThread
{
    Q_OBJECT
public:
    Timecount();
    ~Timecount();

public slots:
   void count();
protected:
    void run();
private:
    QTimer m_Timer;
};

#endif // TIME_H
