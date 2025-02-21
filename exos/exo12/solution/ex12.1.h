
#include <pcosynchro/pcosemaphore.h>

#define CARWEIGHT 1
#define TRUCKWEIGHT 10


class BridgeManager
{
protected:
    PcoSemaphore bridge;
    PcoSemaphore mutex{1};

public:
    BridgeManager(unsigned int maxWeight): bridge(maxWeight)
    {

    }

    ~BridgeManager()
    {

    }

    void carAccess()
    {
        mutex.acquire();
        bridge.acquire();
        mutex.release();
    }

    void truckAccess()
    {
        mutex.acquire();
        for (unsigned int i = 0; i < TRUCKWEIGHT; i++) {
            bridge.acquire();
        }
        mutex.release();
    }

    void carLeave()
    {
        bridge.release();
    }

    void truckLeave()
    {
        for (unsigned int i = 0; i < TRUCKWEIGHT; i++) {
            bridge.release();
        }
    }
};
