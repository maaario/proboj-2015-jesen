
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

#include <complex>
typedef complex<double> Vec;

Vec vecovat(Bod blbec) {
    return Vec(blbec.x, blbec.y);
}

Bod odvecovat(Vec vec) {
    return Bod(vec.real(), vec.imag());
}

Vec norm2(Vec vec) {
    return vec/(abs(vec)+EPS);
}

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

double square(double x)
{
    return x*x;
}

double cube(double x)
{
    return x*x*x;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  Hrac ja = stav.hraci[0];
  Vec mojavec = vecovat(ja.obj.pozicia);
  Vec konRych(0, 0);
  // Bonusy

  for (FyzickyObjekt bonus : stav.obj[ZLATO]) {
      Vec diff = (vecovat(bonus.pozicia) - mojavec);
      konRych += norm2(diff) * (30000000000.0 / cube(abs(diff)+1));
  }
  // Boss
  for (FyzickyObjekt boss : stav.obj[BOSS]) {
      Vec diff = (vecovat(boss.pozicia) - mojavec);
      konRych -= norm2(diff) * (6000000.0 / square(abs(diff)+1));
  }

  // planety
  for (FyzickyObjekt pl : stav.obj[PLANETA]) {
      Vec diff = (vecovat(pl.pozicia) - mojavec);
      konRych -= norm2(diff) * (6000000.0 / square(abs(diff)+1));
  }

  // Steny

  //konRych += Vec(0,1) * (30000000.0/2 / (abs(ja.obj.pozicia.y)+1));
  //konRych += Vec(-1,0) * (30000000.0/2 / (abs(mapa.w - ja.obj.pozicia.x)+1));
  //konRych += Vec(0,-1) * (30000000.0/2 / (abs(mapa.h - ja.obj.pozicia.y)+1));


  double vx= konRych.real();
  double vy= konRych.imag();
  double kmagiaodstien=3000;
  vx=max(min(vx,kmagiaodstien*sqrt(mapa.w-mojavec.real())),-kmagiaodstien*sqrt(mojavec.real()));
  vy=max(min(vy,kmagiaodstien*sqrt(mapa.h-mojavec.imag())),-kmagiaodstien*sqrt(mojavec.imag()));
  konRych=Vec(vx,vy);



  konRych-=vecovat(ja.obj.rychlost);
  double v=abs(konRych);
  v=max(v,-200.0);
  v=min(v,200.0);
  konRych=norm2(konRych)*v;
  prikaz.acc = odvecovat(konRych);
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
