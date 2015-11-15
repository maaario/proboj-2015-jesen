
#ifndef UPDATE_H
#define UPDATE_H

#include <ostream>
#include <map>

#include "common.h"


extern const int DX[4];
extern const int DY[4];

extern const int kSkoreFrag;
extern const int kCasNaNarast;
extern const int kZaciatocnaVelkost;

extern const int kMaximalnaDlzkaHry;


void zapniObservation(std::ostream* observation);

Stav zaciatokHry(const Mapa& mapa);
void odsimulujKolo(const Mapa& mapa, Stav& stav, const std::vector<Odpoved>& akcie);

#endif
