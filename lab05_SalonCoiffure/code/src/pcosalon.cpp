/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Modifications à faire dans le fichier
/**
 * @name Project - PCO Salon
 * @authors Haeffner Edwin, Junod Arthur
 * @date 27.11.23
 */


#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
   : _interface(interface) {
   _capacity = capacity;
   // Initialise le vector de chaises libres ou non
   for (unsigned i = 0; i < capacity; ++i) {
      _chairs.emplace_back(false);
   }
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
bool PcoSalon::accessSalon(unsigned clientId) {
   _mutex.lock();

   //On regarde si toutes les places sont prise, ou non !
   if (_nbInSaloon == _capacity) {
      _mutex.unlock();
      return false;
   }

   _interface->consoleAppendTextClient(clientId, "Je m'engouffre dans la porte, "
                                                 "faites attention !!");
   _nbInSaloon++;
   animationClientAccessEntrance(clientId);

    // Vérifie que le coiffeur ne dorme pas, si oui il le reveille
    if (_isSleeping) {
        std::cout << clientId << " : Debout là dedans !" << std::endl;
        _isSleeping = false;
        _zzzBarber.notifyOne();
    }

    // Le client prend son numéro de ticket
    unsigned myTicket = ++_ticket;
    std::cout << clientId << " : Il est temps d'attendre... order : " << myTicket <<
              std::endl;

    // Le client choisi sur quelle chaise il va s'asseoir
    unsigned chairToSitOn;
    for (unsigned i = 0; i < _capacity; ++i) {
        if (!_chairs[i]) {
            _chairs[i] = true;
            chairToSitOn = i;
            break;
        }
    }
    animationClientSitOnChair(clientId, chairToSitOn);

    // Tous les clients qui ont un ticket plus grand que `order` retourneront en
    // attente
    while (myTicket > _order) {
        _waitingForCut.wait(&_mutex);
    }


    _chairs[chairToSitOn] = false;
    std::cout << clientId << " : OMG je vais me couper les cheveux !" << std::endl;
    _nbInSaloon--;

   _mutex.unlock();
   return true;
}


void PcoSalon::goForHairCut(unsigned clientId) {
   _mutex.lock();
   //Ensuite, il va se faire couper les cheveux
   animationClientSitOnWorkChair(clientId);
   std::cout << clientId << " : Tient, il est confortable ce siège !" << std::endl;
   _waitingForClient.notifyOne();
   _cutting.wait(&_mutex);
   //Une fois que la coiffure est terminée
   std::cout << clientId << " : Merci chef, sniff !" << std::endl;
   _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId) {
   _mutex.lock();
   std::cout << clientId << " : Mes cheveux doivent pousser maintenant !" <<
             std::endl;
   animationClientWaitForHairToGrow(clientId);
   _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId) {
   _mutex.lock();
   std::cout << clientId << " : Pas de place, on se balade :-P" << std::endl;
   animationClientWalkAround(clientId);
   _mutex.unlock();
}


void PcoSalon::goHome(unsigned clientId) {
   _mutex.lock();
   std::cout << clientId << " : A la maison !" << std::endl;
   animationClientGoHome(clientId);
   _mutex.unlock();
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
unsigned int PcoSalon::getNbClient() {
   return (unsigned) _nbInSaloon;
}

void PcoSalon::goToSleep() {
   _mutex.lock();
   // Si on ferme le salon le coiffeur s'endort et débloque le thread de l'app
    animationBarberGoToSleep();
   if (!_isOpen) {
      _waitingToClose.notifyAll();
   } else {
       // Sinon il va juste dormir
      _isSleeping = true;
      std::cout << "Barbier : zzzzz..." << std::endl;
      _zzzBarber.wait(&_mutex);
      std::cout << "Barbier : ouaah.. oui je viens !" << std::endl;
      animationWakeUpBarber();
   }
    _mutex.unlock();
}


void PcoSalon::pickNextClient() {
   _mutex.lock();
   // Incrémente order afin de libérer le client qui a le numéro de ticket le plus bas
   ++_order;
   std::cout << "Barbier : toi ! Viens par là ! Client no : " << _order << std::endl;
   _waitingForCut.notifyAll();
   _mutex.unlock();
}


void PcoSalon::waitClientAtChair() {
   _mutex.lock();
   std::cout << "Barbier : je t'attends là hop hop hop" << std::endl;
   _waitingForClient.wait(&_mutex);
   std::cout << "Alors, tu veux quoi comme coupe ?!" << std::endl;
   _mutex.unlock();
}


void PcoSalon::beautifyClient() {
   _mutex.lock();
   animationBarberCuttingHair();
   _cutting.notifyOne();
   _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService() {
   return _isOpen;
}


void PcoSalon::endService() {
   _mutex.lock();
   _isOpen = false;


   _waitingToClose.wait(&_mutex);

   std::cout << "La journée est terminée, à la revoyure !" << std::endl;
   _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId) {
   _mutex.unlock();
   _interface->clientAccessEntrance(clientId);
   _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb) {
   _mutex.unlock();
   _interface->clientSitOnChair(clientId, (int)clientSitNb);
   _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId) {
   _mutex.unlock();
   _interface->clientSitOnWorkChair(clientId);
   _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId) {
   _mutex.unlock();
   _interface->clientWaitHairToGrow(clientId, true);
   _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId) {
   _mutex.unlock();
   _interface->clientWalkAround(clientId);
   _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep() {
   _mutex.unlock();
   _interface->barberGoToSleep();
   _mutex.lock();
}

void PcoSalon::animationWakeUpBarber() {
   _mutex.unlock();
   _interface->clientWakeUpBarber();
   _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair() {
   _mutex.unlock();
   _interface->barberCuttingHair();
   _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId) {
   _mutex.unlock();
   _interface->clientWaitHairToGrow(clientId, false);
   _mutex.lock();
}
