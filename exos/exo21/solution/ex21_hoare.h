#ifndef READERWRITERCLASSAB_H
#define READERWRITERCLASSAB_H

#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcomanager.h>
#include <pcosynchro/pcosemaphore.h>

#include <iostream>


class ReaderWriterClassAB : PcoHoareMonitor {
    protected:
    Condition a, b;
    unsigned  nbA = 0, nbB = 0, waitingA = 0, waitingB = 0;

    public:
    ReaderWriterClassAB() {
    }

    void lockA() {
        monitorIn();
        if (nbB > 0) {
            ++waitingA;
            wait(a);
            waitingA--;
        }
        
        ++nbA;
        
        if (waitingA > 0) {
            signal(a);
        }
        monitorOut();
    }

    void unlockA() {
        monitorIn();
        --nbA;
        
        if (nbA == 0 && waitingB > 0) {
            signal(b);
        }
        monitorOut();
    }

    void lockB() {
        monitorIn();
        if (nbA > 0) {
            ++waitingB;
            wait(b);
            waitingB--;
        }
        
        ++nbB;
        
        if (waitingB > 0) {
            signal(b);
        }
        monitorOut();
    }

    void unlockB() {
        monitorIn();
        --nbB;
        
        if (nbB == 0 && waitingA > 0) {
            signal(a);
        }
        monitorOut();
    }
};
#endif  // READERWRITERCLASSAB_H
