#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include <pcosynchro/pcosemaphore.h>

#include "abstractbuffer.h"

#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

template<typename T> class Buffer2ConsoMesa : public AbstractBuffer<T> {
protected:
    PcoMutex mutex;
    PcoConditionVariable full, empty;
    T element;
    unsigned nbElement;

public:
    Buffer2ConsoMesa():nbElement(0) {}

    virtual ~Buffer2ConsoMesa() {}

    virtual void put(T item) {
        mutex.lock();

        while (nbElement > 0) {
            full.wait(&mutex);
        }

        element = item;

        nbElement = 2;
        empty.notifyAll();

        mutex.unlock();
    }

    virtual T get(void) {
        T item;
        mutex.lock();

        while(nbElement == 0){
            empty.wait(&mutex);
        }

        item = element;

        --nbElement;

        if(nbElement == 0){
            full.notifyOne();
        }

        mutex.unlock();

        return item;
    }
};

#endif // BUFFER2CONSO_H
