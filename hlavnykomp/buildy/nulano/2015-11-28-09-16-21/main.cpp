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

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
	for (unsigned int i = 1; i < stav.hraci.size(); i++) {
		if (!stav.hraci[i].zije()) continue;
		
	}
	for (unsigned int i = 1; i < stav.obj[STRELA].size(); i++) {
		cerr << "strela " << i;
		cerr << " na pozicii [" << stav.obj[STRELA][i].pozicia.x << ", " << stav.obj[STRELA][i].pozicia.y << "] ";
		cerr << " v smere [" << stav.obj[STRELA][i].rychlost.x << ", " << stav.obj[STRELA][i].rychlost.y << "]" << endl;
	}
	prikaz.ciel = Bod();
	unsigned int i;
	for (i = 1; i < stav.hraci.size() && !stav.hraci[i].zije(); i++) {}
	//{
		FyzickyObjekt str;
		//str = stav.obj[STRELA][0];
		str = stav.hraci[i].obj;
		prikaz.ciel = str.pozicia - stav.hraci[0].obj.pozicia - str.rychlost - stav.hraci[0].obj.rychlost;
	//}
	int r = rand()%360;
	prikaz.acc = Bod(sin(r*PII/180.0)*200,cos(r*PII/180.0)*200);   // akceleracia
	//r = rand()%360;
	//prikaz.ciel = Bod(sin(r*PII/180.0),cos(r*PII/180.0));  // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
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
