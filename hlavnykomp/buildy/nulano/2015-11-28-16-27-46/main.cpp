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
	cerr << "Uhybam sa" << endl;
	for (unsigned int i = 0; i < stav.obj[ASTEROID].size(); i++) {
		if ((stav.obj[ASTEROID][i].pozicia-pozicia).dist() < stav.obj[ASTEROID][i].polomer*3) {
			cielovaPozicia = pozicia + (pozicia-stav.obj[ASTEROID][i].pozicia);
		}
	}
	for (unsigned int i = 0; i < stav.obj[PLANETA].size(); i++) {
		if ((stav.obj[PLANETA][i].pozicia-pozicia).dist() < stav.obj[PLANETA][i].polomer*2) {
			cielovaPozicia = pozicia + (pozicia-stav.obj[PLANETA][i].pozicia);
		}
	}
	/*for (unsigned int i = 0; i < stav.obj[ASTEROID][i].size(); i++) {
		if ((stav.obj[ASTEROID][i].pozicia-pozicia).dist() < stav.obj[ASTEROID][i].polomer*3) {
			cielovaPozicia = pozicia + (pozicia-stav.obj[ASTEROID][i].pozicia);
		}
	}*/
	
	cerr << "Robim si odstup od steny" << endl;
	cerr << "Ciel pred: [" << cielovaPozicia.x << ", " << cielovaPozicia.y << "]" << endl;
	if (cielovaPozicia.x < 100) cielovaPozicia.x = 120;
	if (cielovaPozicia.y < 100) cielovaPozicia.y = 120;
	if (cielovaPozicia.x > mapa.w-100) cielovaPozicia.x = mapa.w-120;
	if (cielovaPozicia.y > mapa.h-100) cielovaPozicia.y = mapa.h-120;
	cerr << "Ciel pred: [" << cielovaPozicia.x << ", " << cielovaPozicia.y << "]" << endl;
	
  Bod rozdielPozicie = cielovaPozicia - pozicia;
  Bod cielovaRychlost = rozdielPozicie * 0.5;
	cerr << "Hladam cestu. cielovaRychlost = [" << cielovaRychlost.x << ", " << cielovaRychlost.y << "]" << endl;
  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
	if (cielovaRychlost.dist() > 1e-6) {
		double pomerKMojejMaximalnejRychlosti = 200.0 / cielovaRychlost.dist();
		if (pomerKMojejMaximalnejRychlosti > 1.0 /*&& !(pomerKMojejMaximalnejRychlosti < 1.0)*/) {
			cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
		}
	}
	cerr << "Hladam cestu. cielovaRychlost = [" << cielovaRychlost.x << ", " << cielovaRychlost.y << "]" << endl;

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  return cielovaAkceleracia;
}

inline bool cmpsgn(double a, double b) {
	return a>0 ? b>0 : b<=0;
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
		if (!stav.hraci[tgti].zije()) mode = false;
	}
	if (!mode) {
		if (stav.hraci[0].zasobnik == 5) {
			cerr << "Calculating nearest target" << endl;
			mode = true;
			unsigned int min;
			double mindist = 789456;
			for (unsigned int i = 1; i < stav.hraci.size(); i++) {
				if (!stav.hraci[i].zije()) continue;
				double d = (stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist();
				if (stav.hraci[0].obj.rychlost.dist() < 1 && stav.hraci[i].obj.rychlost.dist() < 5)
					d /= (6-stav.hraci[i].obj.rychlost.dist());
				if (d < mindist) {
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
		if (stav.obj[ZLATO][i].rychlost.dist() > 50) { if (kam < 0) {
			kam = i; mindist = ((stav.hraci[0].obj.pozicia - stav.obj[ZLATO][i].pozicia).dist()+100)*1.3;
		} continue; }
		if ((stav.hraci[0].obj.pozicia - stav.obj[ZLATO][i].pozicia).dist() < mindist) {
			kam = i; mindist = (stav.hraci[0].obj.pozicia - stav.obj[ZLATO][i].pozicia).dist();
		}
	}
	cerr << "To bonus " << kam << endl;
	Bod bkam = stav.hraci[0].obj.pozicia;
	if (kam >= 0) {
		bkam = stav.obj[ZLATO][kam].pozicia + (stav.obj[ZLATO][kam].rychlost*.4);
		Bod d = (bkam-stav.hraci[0].obj.pozicia);
		d = (d*(1/d.dist()))*200;
		bkam = bkam + d;
	}
	if (stav.obj[BOSS].size()>0 && (stav.hraci[0].obj.pozicia-stav.obj[BOSS][0].pozicia).dist() < 1000) {
		cerr << "Evade the boss instead" << endl;
		bkam = (stav.hraci[0].obj.pozicia - stav.obj[BOSS][0].pozicia)*1000;
	}
	prikaz.acc = kontroler(stav.hraci[0].obj.pozicia,stav.hraci[0].obj.rychlost,bkam);
	cerr << "Going to [" << bkam.x << ", " << bkam.y <<"] at acc [" << prikaz.acc.x << ", " << prikaz.acc.y << "]" << endl;
	
	cerr << "Aiming" << endl;
	FyzickyObjekt tgt = stav.hraci[tgti].obj;
	/*
	cerr << "Defend?" << endl;
	mindist = 789456;
	for (unsigned int i = 0; i < stav.obj[STRELA].size(); i++) {
		Bod S = stav.obj[STRELA][i].pozicia, s = stav.obj[STRELA][i].rychlost, l = stav.hraci[0].obj.pozicia;
		Bod dst = l-S; //if (cmpsgn(dst.x,s.x) || cmpsgn(dst.y,s.y)) continue;
		cerr << "l-s = [" << dst.x << ", " << dst.y << "]   s = [" << s.x << ", " << s.y << "]" << endl;
		double d = abs((S.x+s.x-l.x)*(S.y-l.y)-(S.y+s.y-l.y)*(S.x-l.x))/s.dist();
		if (mindist > d) { mindist = d; kam = i; }
	}
	if (mindist < 20) {
		tgt = stav.obj[STRELA][kam];
		cerr << "Defend from shot " << kam << " " << mindist << " away at [" << tgt.pozicia.x << ", " << tgt.pozicia.y << "] going [" << tgt.rychlost.x << ", " << tgt.rychlost.y << "]" << endl;
	}
	*/
	if (mode)
		prikaz.ciel = tgt.pozicia - stav.hraci[0].obj.pozicia + (tgt.rychlost - stav.hraci[0].obj.rychlost - (prikaz.acc*DELTA_TIME))/**/;
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
