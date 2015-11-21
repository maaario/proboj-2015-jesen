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

Bod smer(0,0);

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  double x= rand()%3 - 1;
  double y= rand()%3 - 1;
  Bod nahodny(x,y);
  smer= smer+nahodny;
  
  prikaz.acc=smer;
  prikaz.ciel= stav.hraci[1].obj.pozicia;
  prikaz.pal= -1;
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
