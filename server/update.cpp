
#include <ostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

#include "common.h"
#include "update.h"
#include "util.h"

static ostream* g_observation;
void zapniObservation(ostream* observation) { g_observation = observation; }

void odsimuluj(const Mapa& mapa, Stav& stav, const vector<Prikaz>& akcie, long long zmena_cas) {
  double dt = zmena_cas/POMALOST_CASU;
  Stav old_stav = stav;
  stav.time += zmena_cas;

  int kolkaty=0;
  for (Hrac hrac: stav.hraci) {
    Bod akceleracia = akcie[kolkaty].akceleracia;
    if (akceleracia.dist() > MAX_ACCEL) {
      akceleracia = akceleracia*(akceleracia.dist()/MAX_ACCEL);
    }
    hrac.pozicia = hrac.pozicia + akceleracia*dt;
    kolkaty++;
  }
}

void pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov) {
  stav = Stav();
  stav.time = gettime();
  random_shuffle(mapa.spawny.begin(),mapa.spawny.end());
  for (int i=0; i<pocKlientov; i++) {
    Bod start = mapa.spawny[i].pozicia;
    Hrac miso(start.x, start.y, 0, 0);
    stav.hraci.push_back(miso);
  }
  mapa.spawny.clear(); //uz nepotrebne
}
