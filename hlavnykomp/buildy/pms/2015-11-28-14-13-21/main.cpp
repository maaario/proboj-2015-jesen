
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

  DoStuff(konRych, square,   ZLATO,  3000000, mojavec);
  DoStuff(konRych, square, BOSS,    -6000000,      mojavec);
  DoStuff(konRych, square, PLANETA, -6000000,      mojavec);
  DoStuff(konRych, square, ASTEROID,-6000000,      mojavec);

  // ast

  //konRych += Vec(0,1) * (30000000.0/2 / (abs(ja.obj.pozicia.y)+1));
  //konRych += Vec(-1,0) * (30000000.0/2 / (abs(mapa.w - ja.obj.pozicia.x)+1));
  //konRych += Vec(0,-1) * (30000000.0/2 / (abs(mapa.h - ja.obj.pozicia.y)+1));


  double vx= konRych.real();
  double vy= konRych.imag();
  double kmagiaodstien=3000;
  vx=max(min(vx,kmagiaodstien*sqrt(mapa.w-mojavec.real())-5),-kmagiaodstien*sqrt(mojavec.real())+5);
  vy=max(min(vy,kmagiaodstien*sqrt(mapa.h-mojavec.imag())-5),-kmagiaodstien*sqrt(mojavec.imag())+5);
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
