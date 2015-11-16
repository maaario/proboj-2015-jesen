
#include <ostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

#include "common.h"
#include "update.h"

static ostream* g_observation;
void zapniObservation(ostream* observation) { g_observation = observation; }

Stav zaciatokHry(const Mapa& mapa) {
}

void odsimulujKolo(const Mapa& mapa, Stav& stav, const vector<Odpoved>& akcie) {
}

bool hraSkoncila(const Mapa& mapa, const Stav& stav) {
  return false;
}
