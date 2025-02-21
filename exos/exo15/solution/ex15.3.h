#ifndef BUFFER2CONSO_H
#define BUFFER2CONSO_H

#include <pcosynchro/pcosemaphore.h>

#include "abstractbuffer.h"

#include <pcosynchro/pcohoaremonitor.h>

template<typename T> class Buffer2ConsoHoare : public AbstractBuffer<T>, public PcoHoareMonitor {
protected:
    static const unsigned MAX_NBCONSUME = 2;
    Condition notFull, notEmpty;
    T element;
    bool bufferFull;
    unsigned nbConsume;

public:
    Buffer2ConsoHoare() : bufferFull(false), nbConsume(0) {}

    virtual ~Buffer2ConsoHoare() {}

    virtual void put(T item) {
        monitorIn();

        if(bufferFull){
            wait(notFull);
        }
        element = item;
        bufferFull = true;
        signal(notEmpty);
        monitorOut();
    }
    virtual T get(void) {
        T result;
        monitorIn();

        if(!bufferFull){
            wait(notEmpty);
        }
        result = element;
        ++nbConsume;

        if(nbConsume == MAX_NBCONSUME){
            nbConsume = 0;
            bufferFull = false;
            signal(notFull);
        } else {
            signal(notEmpty);
        }

        monitorOut();
        return result;
    }
};


#endif // BUFFER2CONSO_H
