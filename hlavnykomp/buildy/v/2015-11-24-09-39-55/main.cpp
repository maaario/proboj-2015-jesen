#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

Bod kam(-1,-1);

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  if (kam.x<0 || kam.y<0 || kam.x>mapa.w || kam.y>mapa.h) {
    kam=stav.hraci[0].obj.pozicia;
  }
  double dx= rand()%3 - 1;
  double dy= rand()%3 - 1;
  kam= kam+Bod(dx,dy);
  prikaz.acc= kam-stav.hraci[0].obj.pozicia;
  
  double best= INF;
  int kto= -1;
  for (int i=1; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    Bod spojnica= stav.hraci[i].obj.pozicia-stav.hraci[0].obj.pozicia;
    if (spojnica.dist()<best) {
      best=spojnica.dist();
      kto=i;
    }
  }
  if (kto!=-1) {
    prikaz.ciel= stav.hraci[kto].obj.pozicia;
  }
  prikaz.pal= VYSTREL_PUSKA;
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
