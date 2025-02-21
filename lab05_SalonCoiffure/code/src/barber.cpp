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


#include "barber.h"
#include <unistd.h>

#include <iostream>

Barber::Barber(GraphicSalonInterface *interface, std::shared_ptr<SalonBarberInterface> salon)
    : _interface(interface), _salon(salon)
{
    _interface->consoleAppendTextBarber("Adieu, prêt à travailler !");
}

void Barber::run()
{
    while(true) {
        //Vérifie s'il y a des clients dans le salon
        if (_salon->getNbClient() > 0) {
            _interface->consoleAppendTextBarber("Wahou !! :o tant de client "
                                                "attendent !!");
        } else {
            // S'il y en a pas, le barbier va dormir
            _salon->goToSleep();
            if(_salon->isInService()){
                _interface->consoleAppendTextBarber("Ah il est deja 20h !!! bonne nuit "
                                                 "les enfants.");
            }else {
                // Si le salon doit fermer, il va dormir et fini son service (sort
                // du while)
                break;
            }
        }
        // Prend le prochain client et le coiffe
        _interface->consoleAppendTextBarber("Prochain clients, les amicis !");
        _salon->pickNextClient();
        _interface->consoleAppendTextBarber("Faut pas hésiter à bouger son popotin >:(");
        _salon->waitClientAtChair();
        _salon->beautifyClient();
        _interface->consoleAppendTextBarber("Et voila tout beau :^)");
    }
    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
