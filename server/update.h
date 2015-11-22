
#ifndef UPDATE_H
#define UPDATE_H

#include <ostream>
#include <map>

#include "common.h"

void zapniObservation(std::ostream* observation, int ft);

void vypis (const Stav& stav) ;

void odsimuluj(Stav& stav, vector<Prikaz>& akcie,const Mapa& mapa);

bool pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov);

#endif
