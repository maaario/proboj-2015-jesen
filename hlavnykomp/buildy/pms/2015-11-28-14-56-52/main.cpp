
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <functional>

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

typedef double (*math_func)(double);

double square(double x)
{
    return x*x;
}

double cube(double x)
{
    return x*x*x;
}

inline void DoStuff(Vec &konRych, math_func func, int _index, double koeficient, Vec mojavec)
{
    for(auto obj : stav.obj[_index])
    {
        Vec diff = (vecovat(obj.pozicia) - mojavec);
        konRych += (norm2(diff) * (koeficient / func(abs(diff) + 1)));
    }
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  Hrac ja = stav.hraci[0];
  Vec mojavec = vecovat(ja.obj.pozicia);
  Vec konRych(0, 0);
  // Bonusy

  DoStuff(konRych, square, ZLATO,    3000000, mojavec);
  DoStuff(konRych, square, BOSS,    -6000000, mojavec);
  DoStuff(konRych, square, ASTEROID,-6000000, mojavec);
  DoStuff(konRych, square, PLANETA, -6000000, mojavec);
  Vec temp;
  DoStuff(temp, [](double d){return d;}, STRELA,  -4000000, mojavec);
  temp *= Vec(0, 1);    //otocit o 90 deg
  konRych += temp;

  //hraci
  for(auto p : stav.hraci)
  {
      if(p.obj.id == ja.obj.id) continue;
      Vec diff = (vecovat(p.obj.pozicia) - mojavec);
      konRych += (norm2(diff) * (-6000000 / square(abs(diff) + 1)));
  }

  double vx= konRych.real();
  double vy= konRych.imag();
  double kmagiaodstien=20000;
  vx=max(min(vx,kmagiaodstien*sqrt(max(0.0,mapa.w-8*LOD_POLOMER-mojavec.real()))-15),-kmagiaodstien*sqrt(max(0.0,mojavec.real()-8*LOD_POLOMER))+15);
  vy=max(min(vy,kmagiaodstien*sqrt(max(0.0,mapa.h-8*LOD_POLOMER-mojavec.imag()))-15),-kmagiaodstien*sqrt(max(0.0,mojavec.imag()-8*LOD_POLOMER))+15);
  konRych=Vec(vx,vy);

  konRych-=vecovat(ja.obj.rychlost);
  double v=abs(konRych);
  v=max(v,-200.0);
  v=min(v,200.0);
  konRych=norm2(konRych)*v;
  prikaz.acc = odvecovat(konRych);

  //STRIELAJ
    //strielaj na strelu co ide na teba

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
