#ifndef PCOBARRIER_H
#define PCOBARRIER_H

#include <pcosynchro/pcosemaphore.h>

class PcoBarrier
{
    unsigned int nbToWait;
    unsigned int nb{0};
    PcoSemaphore waiting{0};
    PcoSemaphore mutex{1};

public:
    PcoBarrier(unsigned int nbToWait): nbToWait(nbToWait)
    {
    }

    ~PcoBarrier()
    {
    }

    void wait()
    {
        mutex.acquire();
        nb++;
        if (nb < nbToWait) {
            // attente
            mutex.release();
            waiting.acquire();
        } else {
            // débloquer tous les (nbToWait - 1) threads
            for (unsigned int i = 0; i < nbToWait - 1; i++) {
                waiting.release();
            }
            mutex.release();
        }
    }
};

#endif // PCOBARRIER_H
