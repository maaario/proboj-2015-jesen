
#ifndef UPDATE_H
#define UPDATE_H

#include <ostream>
#include <map>

#include "common.h"


// parametre asteroidu
#define AST_MIN_R       7.0 // minimalny polomer asteroidu po rozpade
#define AST_KOLIZNY_LV  10
#define AST_DROP_RATE   0.4
#define AST_ROZPAD_ACC  30.0
#define AST_ZIV_RATE    0.25

// parametre planet
#define PLANETA_KOLIZNY_LV  100
#define PLANETA_ZIV_RATE    20000

// parametre zlata
#define ZLATO_KOLIZNY_LV  1
#define ZLATO_ZIVOTY      10.0

// parametre bossa
#define BOSS_KOLIZNY_LV 100
#define BOSS_ZIVOTY     INF
#define BOSS_PERIODA    30.0

// parametre zbrane
#define STRELA_KOLIZNY_LV 5
#define STRELA_ZIVOTY     EPS

// ine konstanty
#define DELTA_TIME 0.01
#define FRAME_TIME 0.04


void zapniObservation(std::ostream* observation, double ft);

void vypis (const Stav& stav) ;

double rand_float (double d) ;
double rand_float (double l, double r) ;

double casDoZraz (Bod relpoz, Bod relvel, double polomer) ;

void zoznamObjekty (Stav& stav, vector<FyzickyObjekt*>& objekty) ;

void odsimuluj(Stav& stav, vector<Prikaz>& akcie,const Mapa& mapa, double dt);

bool pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov);

#endif
