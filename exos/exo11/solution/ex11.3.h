#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcohoaremonitor.h>

class PcoBarrier : PcoHoareMonitor
{
    unsigned int nbToWait;
    Condition cond;

public:
    PcoBarrier(unsigned int nbToWait): nbToWait(nbToWait)
    {
    }

    ~PcoBarrier(){}

    void wait()
    {
       // Entering the monitor
       monitorIn();
       // Decrementing the count of threads yet to wait
       nbToWait--;
       // If there are still threads left to wait, block this thread using the monitor's condition
       if(nbToWait > 0){
           PcoHoareMonitor::wait(cond);
       }
       // Signal to wake up any waiting threads
       signal(cond);
       // Exiting the monitor
       monitorOut();
    }
};

#endif // PCOBARRIER_H
