#ifndef READERWRITERPRIOWRITERGENERAL_H
#define READERWRITERPRIOWRITERGENERAL_H

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>

#include "abstractreaderwriter.h"

class ReaderWriterPrioWriterGeneral : public AbstractReaderWriter {
protected:
    PcoConditionVariable attenteEcriture;
    PcoConditionVariable attenteLecture;
    PcoMutex mutex;

    int nbLecteurs;
    int nbLecteursEnAttente;
    int nbRedacteursEnAttente;
    bool redactionEnCours;

public:
    ReaderWriterPrioWriterGeneral() :
        nbLecteurs(0),
        nbLecteursEnAttente(0),
        nbRedacteursEnAttente(0),
        redactionEnCours(false)
    {}

    void lockReading() {
        mutex.lock();
        if ((nbRedacteursEnAttente > 0) || (redactionEnCours)) {
            nbLecteursEnAttente ++;
            attenteLecture.wait(&mutex);
        }
        else
            nbLecteurs ++;
        mutex.unlock();
    }

    void unlockReading() {
        mutex.lock();
        nbLecteurs --;
        if (nbLecteurs == 0) {
            if (nbRedacteursEnAttente > 0) {
                redactionEnCours = true;
                nbRedacteursEnAttente --;
                attenteEcriture.notifyOne();
            }
        }
        mutex.unlock();
    }

    void lockWriting() {
        mutex.lock();
        if ((nbLecteurs > 0) || (redactionEnCours)) {
            nbRedacteursEnAttente ++;
            attenteEcriture.wait(&mutex);
        }
        else
            redactionEnCours = true;
        mutex.unlock();
    }

    void unlockWriting() {
        mutex.lock();
        redactionEnCours = false;
        if (nbRedacteursEnAttente) {
            nbRedacteursEnAttente --;
            redactionEnCours = true;
            attenteEcriture.notifyOne();
        }
        else {
            for(int i=0;i<nbLecteursEnAttente; i++) {
                attenteLecture.notifyOne();
            }
            nbLecteurs = nbLecteursEnAttente;
            nbLecteursEnAttente = 0;

        }
        mutex.unlock();
    }
};
#endif// READERWRITERPRIOWRITERGENERAL_H
