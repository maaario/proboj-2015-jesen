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

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  vector<FyzikalnyObjekt*> objekty;
  zoznamObjekty(stav,objekty);

  double worst= 0.0;
  bool uhynam= false;
  Bod uhni;
  for (FyzikalnyObjekt* ptr : objekty) {
    if (!ptr->zije() || ptr->id==stav.hraci[0].obj.id) {
      continue;
    }
    Bod relpoz= ptr->pozicia - stav.hraci[0].obj.pozicia;
    Bod relvel= stav.hraci[0].obj.rychlost - ptr->rychlost;
    double sumpolomer= stav.hraci[0].obj.polomer + ptr->polomer;
    double cas= casDoZraz(relpoz, relvel, sumpolomer);
    double priorita= ptr->sila/cas;
    if (priorita <= worst || cas<=0 || cas>=2.0) {
      continue;
    }
    worst= priorita;
    Bod pata= relpoz.pata(relvel);
    Bod kolm= pata - relpoz;
    if (kolm==Bod()) {
      kolm= relpoz*(-1);
    }
    uhni= kolm*(LOD_MAX_ACC/kolm.dist());
    uhynam= true;
  }
  if (uhynam) {
    prikaz.acc= uhni;
  }
  else {
    prikaz.acc= Bod();
  }

  // strielanie
  double mindist= INF;
  Bod tgt;
  for (FyzikalnyObjekt* ptr : objekty) {
    if (!ptr->zije() || ptr->id==stav.hraci[0].obj.id) {
      continue;
    }
    if (ptr->typ>=PROJ_BEGIN && ptr->typ!=LOD) {
      continue;
    }
    Bod relpoz= ptr->pozicia - stav.hraci[0].obj.pozicia;
    if (relpoz.dist() < mindist) {
      mindist= relpoz.dist();
      tgt= relpoz;
    }
  }
  prikaz.ciel= stav.hraci[0].obj.pozicia + tgt;
  prikaz.pal= VYSTREL_BOMBA;
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
