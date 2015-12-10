#include <iostream>
#include <unistd.h>
#include <cmath>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

double cielovaVelkostRychlosti = 300;
double maxUhol = 40*PII/180;

double vektorovySucin(Bod bod1, Bod bod2) {
  return bod1.x*bod2.y-bod1.y*bod2.x;
}

Bod otocene(Bod bod, double uhol) {
  return Bod(bod.x*cos(uhol)-bod.y*sin(uhol), bod.x*sin(uhol)+bod.y*cos(uhol));
}

Bod normalizovane(Bod bod) {
  double dist = bod.dist();
  return Bod(bod.x/dist, bod.y/dist);
}

double uhol(Bod bod1, Bod bod2) {
  return asin(vektorovySucin(bod1, bod2)/(bod1.dist()*bod2.dist()));
}

double vzdialenost(Bod bod1, Bod bod2) {
  return abs((bod1-bod2).dist());
}

bool jePriblizneNaPozicii(Bod bod, Bod bod2) {
  
  double dist = (bod2-bod).dist();
  //cerr << dist << endl;
  if(dist<5) return true;
  else return false;
}

Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia) {
  Bod rozdielPozicie = cielovaPozicia - pozicia;
  Bod cielovaRychlost = normalizovane(rozdielPozicie)*cielovaVelkostRychlosti;

  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
  double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
  if (pomerKMojejMaximalnejRychlosti > 1.0) {
    cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
  }

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 10.0;
  return cielovaAkceleracia;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;

Bod ciel = Bod();

void zistiTah() {
  Hrac* hrac = &stav.hraci[0];
  
  cerr << "rychlost: " << hrac->obj.rychlost.x << " " << hrac->obj.rychlost.y << ", velkost: " << hrac->obj.rychlost.dist() << endl;
  cerr << "pozicia: " << hrac->obj.pozicia.x << " " << hrac->obj.pozicia.y << endl;
  
  Bod najblizsieZlato = Bod();
  for(auto obj = stav.obj[ZLATO].begin(); obj!=stav.obj[ZLATO].end(); ++obj) {
    if(najblizsieZlato == Bod())najblizsieZlato = obj->pozicia;
    if(vzdialenost(hrac->obj.pozicia, obj->pozicia) < vzdialenost(hrac->obj.pozicia, najblizsieZlato)) najblizsieZlato = obj->pozicia;
  }
  ciel = najblizsieZlato;
  if(ciel == Bod(0,0)|| jePriblizneNaPozicii(hrac->obj.pozicia, ciel)) {
    ciel = Bod((rand()%1000)+500, (rand()%1000)+500);
    cerr << "novy ciel: " << ciel.x << " " << ciel.y << endl;
    cerr << "aktulna pozicia: " << hrac->obj.pozicia.x << " " << hrac->obj.pozicia.y << endl;
    cerr << "rozdiel: " << (ciel-hrac->obj.pozicia).x << " " << (ciel-hrac->obj.pozicia).y << endl;
  }
  Bod cielovaRychlost;
  if (!(hrac->obj.rychlost == Bod(0,0))){
    cielovaRychlost = normalizovane(hrac->obj.rychlost)*cielovaVelkostRychlosti;
    double uhol1 = uhol(hrac->obj.rychlost, ciel - hrac->obj.pozicia);
    cerr << "uhol: " << uhol1 << endl;
    cielovaRychlost = otocene(cielovaRychlost, /*min(uhol1/abs(uhol1)*maxUhol, uhol1)*/uhol1);
  } else {
    cielovaRychlost = normalizovane(ciel - hrac->obj.pozicia)*cielovaVelkostRychlosti;
    //cerr << "uhol: " << uhol(ciel - hrac->obj.pozicia, cielovaRychlost) << endl;
  }
  cerr << "cielova rychlost: " << cielovaRychlost.x << " " << cielovaRychlost.y << endl;
  cerr << "cielova pozicia: " << ciel.x << " " << ciel.y << endl;
  
  Bod akceleracia = (cielovaRychlost - hrac->obj.rychlost)*10;
  cerr << "akceleracia: " << akceleracia.x << " " << akceleracia.y << endl;
  cerr << endl;
  prikaz.acc = akceleracia;
  prikaz.ciel = Bod();  // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
}





















int main() {
  // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

  unsigned seed = time(NULL) * getpid();
  srand(seed);

  nacitaj(cin,mapa);
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);

  while (cin.good()) {
    nacitaj(cin,stav);

    zistiTah();

    uloz(cout,prikaz);
    cout << endl;
  }
}
