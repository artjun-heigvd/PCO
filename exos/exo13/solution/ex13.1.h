
#include <pcosynchro/pcosemaphore.h>


class BridgeManagerFloat
{
    PcoSemaphore mutex{1};
    PcoSemaphore waiting{0};
    PcoSemaphore fifo{1};
    float currentWeight{0.f};
    float maxWeight;
    int nbWaiting{0};

public:
    BridgeManagerFloat(float maxWeight) :
        maxWeight(maxWeight)
    {

    }

    ~BridgeManagerFloat()
    {

    }

    void access(float weight)
    {
        // serialization: un thread tente d'entrer sur le pont à la fois
        fifo.acquire();

        // mutex pour protéger les variables partagées entre access() et leave()
        mutex.acquire();

        // il faut revérifier la condition
        while(currentWeight + weight > maxWeight) {
            // attente en relâchant le mutex
            ++nbWaiting;
            mutex.release();
            waiting.acquire();

            // réacquisition du mutex
            mutex.acquire();
        }

        // accès au pont
        currentWeight += weight;

        mutex.release();
        fifo.release();
    }

    void leave(float weight)
    {
        mutex.acquire();

        // sortie du pont
        currentWeight -= weight;

        // débloquer 0 ou 1 thread
        // (le sémaphore "fifo" garantie d'avoir au plus 1 thread en attente)
        if (nbWaiting > 0) {
            nbWaiting--;
            waiting.release();
        }

        mutex.release();
    }
};
