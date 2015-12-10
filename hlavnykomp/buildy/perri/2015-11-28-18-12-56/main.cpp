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
double accY = -45;
Bod rychlost;
double uhol = 1;


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
    rychlost = stav.hraci[0].obj.rychlost;
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
    bool zmena = false;
    if (ja.x < odX)
    {
        vx = rychlost.x*2;
        zmena = true;
        accX = -1*accX;
    }
    else if ( ja.x > (mapa.w - odX))
    {
        vx = -rychlost.x*2;
        zmena = true;
        accX = -1*accX;
    }

    if (ja.y < odY)
    {
        vy = rychlost.y*2;
        zmena = true;
        accY = -1*accY;
    }
    else if (ja.y > (mapa.h - odY))
    {
        vy = -rychlost.y*2;
        zmena = true;
        accY = -1*accY;
        cerr << "handle down"<<endl;
    }

    cerr << "vx,vy, suX, suY "<< vx<<" "<<vy<<" "<<suX<<" "<<suY<<endl;
    if (!zmena)
    {
        if(rychlost.x > suX)
            vx = -10*suX/abs(suX);

        if(rychlost.y > suY)
            vy = -10*suY/abs(suY);
    } 
    return Bod(vx, vy);
}

void zistiTah() {

  aktualizuj();
  uhol += 0.01;
  cerr << "X, Y ["<<ja.x<<", "<<ja.y<<"] rychlost x,y: ["<<rychlost.x<<", "<<rychlost.y<<"]"<<endl;
  //prikaz.acc = stena(1000+100*cos(stav.hraci[0].obj.pozicia.x), 1000+100*sin(stav.hraci[0].obj.pozicia.y));   // akceleracia
  prikaz.acc = Bod(10*cos(uhol), 10*sin(uhol));
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
