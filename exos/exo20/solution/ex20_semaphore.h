#ifndef READERWRITERPRIOWRITERGENERAL_H
#define READERWRITERPRIOWRITERGENERAL_H

#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcosemaphore.h>

#include "abstractreaderwriter.h"


class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter {
protected:
    PcoSemaphore mutex;

    PcoSemaphore readerBlocker;
    int nbReadersWaiting;

    PcoSemaphore writerBlocker;
    int nbWritersWaiting;

    //Added attributes
    bool oneWriter = false;
    int nbReaders = 0;

    //Rules
    // • Un lecteur peut accéder à la ressource si:
    // 		• Le nombre de rédacteurs en cours d’écriture vaut 0
    // 		• Et le nombre de rédacteurs en attente d’écriture vaut 0
    // • Un rédacteur peut accéder à la ressource si:
    // 		• Le nombre de rédacteurs en cours d’écriture vaut 0
    // 		• Et le nombre de lecteurs en cours de lecture vaut 0
public:
    ReaderWriterPrioWriterGeneral() : mutex(1),
                                      nbReadersWaiting(0),
                                      nbWritersWaiting(0) {}

    void lockReading() {
        mutex.acquire();
        if (oneWriter || nbWritersWaiting > 0) {
            nbReadersWaiting++;
            mutex.release();
            readerBlocker.acquire();
        } else {
            nbReaders++;
            mutex.release();
        }
    }

    void unlockReading() {
        mutex.acquire();
        nbReaders--;
        if (nbReaders == 0 && nbWritersWaiting > 0) {
            oneWriter = true;
            nbWritersWaiting--;
            writerBlocker.release();
        }
        mutex.release();
    }

    void lockWriting() {
        mutex.acquire();
        if (oneWriter || nbReaders > 0) {
            nbWritersWaiting++;
            mutex.release();
            writerBlocker.acquire();
        } else {
            oneWriter = true;
            mutex.release();
        }
    }

    void unlockWriting() {
        mutex.acquire();

        if (nbWritersWaiting > 0) {
            nbWritersWaiting--;
            writerBlocker.release();
        } else {
            oneWriter = false;

            //Release all waiting readers
            for (int i = 0; i < nbReadersWaiting; ++i) {
                readerBlocker.release();
            }
            nbReaders = nbReadersWaiting;
            nbReadersWaiting = 0;
        }

        mutex.release();
    }
};
#endif// READERWRITERPRIOWRITERGENERAL_H
