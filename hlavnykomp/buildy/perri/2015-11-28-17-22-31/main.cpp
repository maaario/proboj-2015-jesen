#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;
Bod ja;
double odX;
double odY;
double accX = 0;
double accY = 0;


Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia) {
  Bod rozdielPozicie = cielovaPozicia - pozicia;
  Bod cielovaRychlost = rozdielPozicie * 0.5;

  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
  double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
  if (pomerKMojejMaximalnejRychlosti > 1.0) {
    cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
  }

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  return cielovaAkceleracia;
}

void aktualizuj()
{
    ja = stav.hraci[0].obj.pozicia;
    Bod rychlost = stav.hraci[0].obj.rychlost;
    odX = 2*rychlost.x;
    odY = 2*rychlost.y;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
Bod najdiNepriatela()
{

}

Bod stena(double suX, double suY)
{
    double vx = suX;
    double vy = suY;
    if (ja.x < odX)
    {
        vx = 200;
    }
    else if ( ja.x > (mapa.w - odX))
    {
        vx = -200;
    }

    if (ja.y < odY)
    {
        vy = 200;
    }
    else if (ja.y > (mapa.h - odY))
    {
        vy = -200;
        cerr << "handle down"<<endl;
    }

    return Bod(vx, vy);
}

void zistiTah() {

  aktualizuj();
  prikaz.acc = stena(0, 45);   // akceleracia
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
