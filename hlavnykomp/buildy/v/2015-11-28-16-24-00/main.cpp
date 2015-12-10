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

Bod najblizsiBonus() {
  Bod ja = stav.hraci[0].obj.pozicia;
  Bod najlepsi(-1, -1);
  bool prvy = true;
  for (FyzickyObjekt &objekt: stav.obj[ZLATO]) {
    if (prvy || (ja - objekt.pozicia).dist() < (ja - najlepsi).dist()) {
      najlepsi = objekt.pozicia;
      prvy = false;
    }
  }
  return najlepsi;
}

Bod najblizsiSuper() {
  Bod ja = stav.hraci[0].obj.pozicia;
  Bod najblizsi(-1, -1);
  bool prvy = true;
  for (Hrac &hrac: stav.hraci) {
    if (!hrac.zije()) continue;
    double d = (ja - hrac.obj.pozicia).dist();
    if (d < 10e-6) continue;
    if (prvy || (ja - hrac.obj.pozicia).dist() < (ja - najblizsi).dist()) {
      najblizsi = hrac.obj.pozicia;
      prvy = false;
    }
  }
  return najblizsi;
}

Bod vypocitajStrielanie(Bod ja, Bod jaRychlost, Bod super, Bod superRychlost) {
  double dolna = 0.0;
  double horna = 20.0;
  while (horna - dolna > 1e-4) {
    double stred = (dolna + horna) / 2.0;
    Bod potom = super + superRychlost * stred;
    double vzdialenost = (ja - potom).dist();
    double cas = vzdialenost / 500.0;
    if (cas < stred) horna = stred;
    else dolna = stred;
  }

  Bod ciel = super + superRychlost * ((dolna + horna) / 2.0) - ja;
  ciel = ciel * (1.0 / ciel.dist());
  ciel = ciel * 500.0;
  ciel = ciel - jaRychlost;

  return ciel;
}

bool strielam = true;

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
    Bod idem = najblizsiBonus();
    if (idem == Bod(-1, -1)) idem = najblizsiSuper();
    if (idem == Bod(-1, -1)) idem = Bod();
    prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, idem);
    if (strielam) {
      Bod super = najblizsiSuper();
      Bod superRychlost;
      for (Hrac &hrac: stav.hraci) {
        if (hrac.obj.pozicia == super) {
          superRychlost = hrac.obj.rychlost;
        }
      }
      prikaz.ciel = vypocitajStrielanie(
              stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost,
              super, superRychlost);
      if (stav.hraci[0].zasobnik == 0) strielam = false;
    } else if (stav.hraci[0].zasobnik == 5) strielam = true;
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
