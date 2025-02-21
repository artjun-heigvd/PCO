#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcoconditionvariable.h>

#define CARWEIGHT 1
#define TRUCKWEIGHT 10

class BridgeManager
{
    PcoMutex mutex;
    PcoConditionVariable condition;

    unsigned int maxWeight;
    unsigned int currentWeight = 0;

public:
    BridgeManager(unsigned int maxWeight) : maxWeight(maxWeight)
    {
    }

    ~BridgeManager()
    {
    }

    void carAccess()
    {
        mutex.lock();
        while (currentWeight + CARWEIGHT > maxWeight) {
            condition.wait(&mutex);
        }
        currentWeight += CARWEIGHT;
        mutex.unlock();
    }

    void truckAccess()
    {
        mutex.lock();
        while (currentWeight + TRUCKWEIGHT > maxWeight) {
            condition.wait(&mutex);
        }
        currentWeight += TRUCKWEIGHT;
        mutex.unlock();
    }

    void carLeave()
    {
        mutex.lock();
        currentWeight -= CARWEIGHT;
        condition.notifyAll();
        mutex.unlock();
    }

    void truckLeave()
    {
        mutex.lock();
        currentWeight -= TRUCKWEIGHT;
        condition.notifyAll();
        mutex.unlock();
    }
};
