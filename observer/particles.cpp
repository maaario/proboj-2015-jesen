#include <random>
#include <cmath>
#include "particles.h"

using namespace std;

CasticovySystem::generuj(Bod pozicia, double maxRychlost, int pocet) {
  for (int i = 0; i < pocet; ++i) {
    Castica castica;
    castica.pozicia = pozicia;

    double rychlost = uniform_real_distribution(0.0, maxRychlost);
    double smer = uniform_real_distribution(
  }
}
