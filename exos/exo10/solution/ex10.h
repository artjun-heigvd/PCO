#include <pcosynchro/pcosemaphore.h>
#include <iostream>


class MutexFromSem
{
protected:
    bool locked{false};
    PcoSemaphore waiting{0};
    unsigned int nbWaiting{0};
    PcoSemaphore mutex{1};
public:
    MutexFromSem()
    {

    }

    ~MutexFromSem()
    {

    }

    void lock()
    {
        mutex.acquire();
        if(locked) {
            // bloquer
            nbWaiting++;
            mutex.release();
            waiting.acquire();
            mutex.acquire();

        } else {
            locked = true;
        }
        mutex.release();
    }

    void unlock()
    {
        mutex.acquire();
        if(nbWaiting > 0) {
            // débloque
            waiting.release();
            nbWaiting--;
        } else {
            locked = false;
        }
        mutex.release();
    }

    bool trylock()
    {
        mutex.acquire();

        if(locked) {
            mutex.release();
            return false;
        } else {
            locked = true;
            mutex.release();
            return true;
        }



    }  //! Returns true if the mutex can be acquired, false if it is already locked
};
