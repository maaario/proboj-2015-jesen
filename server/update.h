
#ifndef UPDATE_H
#define UPDATE_H

#include <ostream>
#include <map>

#include "common.h"

void zapniObservation(std::ostream* observation, double ft);

void vypis (const Stav& stav) ;

double casDoZraz (Bod relpoz, Bod relvel, double polomer) ;

void zoznamObjekty (Stav& stav, vector<FyzikalnyObjekt*>& objekty) ;

void odsimuluj(Stav& stav, vector<Prikaz>& akcie,const Mapa& mapa, double dt);

bool pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov);

#endif
