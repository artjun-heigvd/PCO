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

#include "client.h"

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface *interface,
               std::shared_ptr<SalonClientInterface> salon)

   : _interface(interface), _salon(salon), _clientId(_nextId++) {
   _interface->consoleAppendTextClient(_clientId, "Adieu, prêt pour une coupe !");
}

void Client::run() {
   uint clientId = (unsigned) _clientId; //Pour éviter de recast + éviter warning cast
   // implicite

   while (true) {
      // Vérifie que le salon soit ouvert
      if (!_salon->isInService()) {
         _interface->consoleAppendTextClient(clientId,
                                             "Le salon est fermé... Zut !");
         _salon->goHome(clientId);
         break;
      }
      if (!_salon->accessSalon(clientId)) {
          // Si le salon est plein le client tourne en rond en attendant !
         _salon->walkAround(clientId);
         _interface->consoleAppendTextClient(clientId,
                                             "Le salon est plein BAH MERCREDI PUREE"
                                             " MES CHEVEUX QUOI...");
         continue;
      }
      // Le client rentre dans le salon
      _salon->goForHairCut(clientId);
      _salon->waitingForHairToGrow(clientId);
   }
}




