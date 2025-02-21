#ifndef READERWRITERPRIOWRITERGENERAL_H
#define READERWRITERPRIOWRITERGENERAL_H

#include <pcosynchro/pcohoaremonitor.h>
#include <pcosynchro/pcosemaphore.h>

#include "abstractreaderwriter.h"


class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter, PcoHoareMonitor {
protected:
    Condition readerBlocker;
    Condition writerBlocker;
    int nbWritersWaiting;
    bool oneWriter = false;
    int nbReaders = 0;

public:
    ReaderWriterPrioWriterGeneral() :
        nbWritersWaiting(0) {}

    void lockReading() {
        monitorIn();

        if (oneWriter || nbWritersWaiting > 0) {
            wait(readerBlocker);
            signal(readerBlocker);
        }

        nbReaders++;

        monitorOut();
    }

    void unlockReading() {
        monitorIn();

        nbReaders--;
        if (nbReaders == 0 && nbWritersWaiting > 0) {
            oneWriter = true;
            nbWritersWaiting--;
            signal(writerBlocker);
        }

        monitorOut();
    }

    void lockWriting() {
        monitorIn();

        if (oneWriter || nbReaders > 0) {
            nbWritersWaiting++;
            wait(writerBlocker);
        }

        oneWriter = true;

        monitorOut();
    }

    void unlockWriting() {
        monitorIn();

        if (nbWritersWaiting > 0) {
            nbWritersWaiting--;
            signal(writerBlocker);
        } else {
            oneWriter = false;
            signal(readerBlocker);
        }

        monitorOut();
    }
};
#endif// READERWRITERPRIOWRITERGENERAL_H
