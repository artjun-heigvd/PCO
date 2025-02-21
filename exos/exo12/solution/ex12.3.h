
#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcohoaremonitor.h>

#define CARWEIGHT 1
#define TRUCKWEIGHT 10


class BridgeManager : public PcoHoareMonitor
{
    const unsigned MAX_WEIGHT;
    unsigned actualWeight;

    Condition bridgeFull;

public:
    BridgeManager(unsigned int maxWeight): MAX_WEIGHT(maxWeight),actualWeight(0)
    {

    }

    ~BridgeManager()
    {

    }

    void carAccess()
    {
        monitorIn();
        
        while (actualWeight + CARWEIGHT > MAX_WEIGHT){
            wait(bridgeFull);
        }

        actualWeight += CARWEIGHT;
        signal(bridgeFull);

        monitorOut();
    }

    void truckAccess()
    {
        monitorIn();

        while (actualWeight + TRUCKWEIGHT > MAX_WEIGHT){
            wait(bridgeFull);
        }

        actualWeight += TRUCKWEIGHT;
        signal(bridgeFull);

        monitorOut();
    }

    void carLeave()
    {
        monitorIn();

        actualWeight -= CARWEIGHT;
        signal(bridgeFull);

        monitorOut();
    }

    void truckLeave()
    {
        monitorIn();

        actualWeight -= TRUCKWEIGHT;
        signal(bridgeFull);

        monitorOut();
    }





};
