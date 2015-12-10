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
	cerr << "Hladam cestu. cielovaRychlost = [" << cielovaRychlost.x << ", " << cielovaRychlost.y << "]" << endl;
  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
	if (cielovaRychlost.dist() > 1e-6) {
		double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
		if (pomerKMojejMaximalnejRychlosti > 1.0 && !(pomerKMojejMaximalnejRychlosti < 1.0)) {
			cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
		}
	}
	cerr << "Hladam cestu. cielovaRychlost = [" << cielovaRychlost.x << ", " << cielovaRychlost.y << "]" << endl;

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  return cielovaAkceleracia;
}

// f - loading; t - shooting
bool mode = false;
unsigned int tgti;

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
	if (mode) {
		if (stav.hraci[0].zasobnik <= 1) {
			cerr << "Out of ammo" << endl;
			mode = false;
		}
	} else {
		if (stav.hraci[0].zasobnik == 5) {
			cerr << "Calculating nearest target" << endl;
			mode = true;
			unsigned int min;
			double mindist = 789456;
			for (unsigned int i = 1; i < stav.hraci.size(); i++) {
				if (!stav.hraci[i].zije()) continue;
				if ((stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist() < mindist) {
					min = i; mindist = (stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist();
				}
			}
			tgti = min;
			cerr << "Chosen target: " << tgti << endl;
		}
	}/*
	for (unsigned int i = 1; i < stav.obj[STRELA].size(); i++) {
		cerr << "strela " << i;
		cerr << " na pozicii [" << stav.obj[STRELA][i].pozicia.x << ", " << stav.obj[STRELA][i].pozicia.y << "] ";
		cerr << " v smere [" << stav.obj[STRELA][i].rychlost.x << ", " << stav.obj[STRELA][i].rychlost.y << "]" << endl;
	}*/
	
	cerr << "Where to go?" << endl;
	int kam = -1; double mindist = 789456;
	for (unsigned int i = 0; i < stav.obj[ZLATO].size(); i++) {
		if (stav.obj[ZLATO][i].rychlost.dist() > 50) continue;
		if ((stav.hraci[0].obj.pozicia - stav.obj[ZLATO][i].pozicia).dist() < mindist) {
			kam = i; mindist = (stav.hraci[0].obj.pozicia - stav.obj[ZLATO][i].pozicia).dist();
		}
	}
	cerr << "To bonus " << kam << endl;
	Bod bkam = stav.hraci[0].obj.pozicia;
	if (kam >= 0) {
		bkam = stav.obj[ZLATO][kam].pozicia;
		Bod d = (bkam-stav.hraci[0].obj.pozicia);
		d = (d*(1/d.dist()))*200;
		bkam = bkam + d;
	}
	prikaz.acc = kontroler(stav.hraci[0].obj.pozicia,stav.hraci[0].obj.rychlost,bkam);
	cerr << "Going to [" << bkam.x << ", " << bkam.y <<"] at acc [" << prikaz.acc.x << ", " << prikaz.acc.y << "]" << endl;
	
	cerr << "Aiming" << endl;
	FyzickyObjekt tgt = stav.hraci[tgti].obj;
	//cerr << "Defend?" << endl;
	if (mode)
		prikaz.ciel = tgt.pozicia - stav.hraci[0].obj.pozicia + tgt.rychlost - stav.hraci[0].obj.rychlost - prikaz.acc/**/;
	else prikaz.ciel = Bod();
	cerr << "Aim: [" << prikaz.ciel.x << ", " << prikaz.ciel.y << "]" << endl;
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
