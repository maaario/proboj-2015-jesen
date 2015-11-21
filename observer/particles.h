#pragma once

#include <vector>
#include "common.h"

using namespace std;

struct Castica {
  Bod pozicia;
  double polomer;

  Bod rychlost;
  double zivot;
};

struct CasticovySystem {
  vector<Castica> castice;

  void generuj(Bod pozicia, double maxPolomer, double maxRychlost, int pocet);
  void aktualizuj(double dt);
};
