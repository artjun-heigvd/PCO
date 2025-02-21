#ifndef READERWRITERCLASSAB_H
#define READERWRITERCLASSAB_H

#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

#include <iostream>

class ReaderWriterClassAB
{
protected:
    PcoMutex mutex;
    PcoConditionVariable attenteA;
    int nbAttenteA;
    PcoConditionVariable attenteB;
    int nbAttenteB;
    int nbA;
    int nbB;

public:
    ReaderWriterClassAB() :
        nbAttenteA(0),
        nbAttenteB(0),
        nbA(0),
        nbB(0) {
    }

    void lockA() {
        mutex.lock();
        if (nbB > 0) {
            nbAttenteA ++;
            attenteA.wait(&mutex);
        }
        else
            nbA ++;
        mutex.unlock();
    }

    void unlockA() {
        mutex.lock();
        nbA --;
        if (nbA == 0) {
            attenteB.notifyAll();
            nbB = nbAttenteB;
            nbAttenteB = 0;
        }
        mutex.unlock();
    }

    void lockB() {
        mutex.lock();
        if (nbA > 0) {
            nbAttenteB ++;
            attenteB.wait(&mutex);
        }
        else
            nbB ++;
        mutex.unlock();

    }

    void unlockB() {
        mutex.lock();
        nbB --;
        if (nbB == 0) {
            attenteA.notifyAll();
            nbA = nbAttenteA;
            nbAttenteA = 0;
        }
        mutex.unlock();
    }
};

#endif  // READERWRITERCLASSAB_H
