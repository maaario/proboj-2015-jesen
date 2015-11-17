
#ifndef UPDATE_H
#define UPDATE_H

#include <ostream>
#include <map>

#include "common.h"

void zapniObservation(std::ostream* observation);

void odsimuluj(const Mapa& mapa, Stav& stav, const vector<Prikaz>& akcie, long long zmena_cas);

void pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov);

#endif
