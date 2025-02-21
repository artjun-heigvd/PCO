#ifndef READERWRITERCLASSAB_H
#define READERWRITERCLASSAB_H

#include <pcosynchro/pcosemaphore.h>

#include <iostream>

class ReaderWriterClassAB {
protected:
    PcoSemaphore mutex;

    PcoSemaphore attenteA;
    int nbAttenteA;

    PcoSemaphore attenteB;
    int nbAttenteB;

    int nbA;
    int nbB;

public:
    ReaderWriterClassAB() :
        mutex(1),
        nbAttenteA(0),
        nbAttenteB(0),
        nbA(0),
        nbB(0) {
    }

    void lockA() {
        mutex.acquire();
        if (nbB > 0) {
            nbAttenteA ++;
            mutex.release();
            attenteA.acquire();
        }
        else {
            nbA ++;
            mutex.release();
        }
    }

    void unlockA() {
        mutex.acquire();
        nbA --;
        if (nbA == 0) {
            for(int i=0;i < nbAttenteB ; i++)
                attenteB.release();
            nbB = nbAttenteB;
            nbAttenteB = 0;
        }
        mutex.release();
    }

    void lockB() {
        mutex.acquire();
        if (nbA > 0) {
            nbAttenteB ++;
            mutex.release();
            attenteB.acquire();
        }
        else {
            nbB ++;
            mutex.release();
        }

    }

    void unlockB() {
        mutex.acquire();
        nbB --;
        if (nbB == 0) {
            for(int i=0;i < nbAttenteA ; i++)
                attenteA.release();
            nbA = nbAttenteA;
            nbAttenteA = 0;
        }
        mutex.release();
    }
};

#endif  // READERWRITERCLASSAB_H
