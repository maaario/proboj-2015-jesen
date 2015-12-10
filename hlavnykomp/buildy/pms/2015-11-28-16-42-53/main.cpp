
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

  DoStuff(konRych, square, ZLATO,    15000000, mojavec);
  Vec tempboss;
  DoStuff(tempboss, square, BOSS, -1200000, mojavec);
  tempboss*=Vec(0,1);
  konRych+=tempboss;
  DoStuff(konRych, square, ASTEROID,-6000000, mojavec);
  DoStuff(konRych, square, PLANETA, -6000000, mojavec);
  //Vec temp;
  //DoStuff(temp, [](double d){return d;}, STRELA,  -40000, mojavec);
  //temp *= Vec(0, 1);    //otocit o 90 deg
  //konRych += temp;

  //hraci
  for(auto p : stav.hraci)
  {
      if(p.obj.id == ja.obj.id) continue;
      Vec diff = (vecovat(p.obj.pozicia) - mojavec);
      konRych += (norm2(diff) * (-6000000 / square(abs(diff) + 1)));
  }

  double vx= konRych.real();
  double vy= konRych.imag();
  double kmagiaodstien=80000000;
  vx=max(min(vx,kmagiaodstien*(max(0.0,mapa.w-8*LOD_POLOMER-mojavec.real()))-15),-kmagiaodstien*(max(0.0,mojavec.real()-8*LOD_POLOMER))+15);
  vy=max(min(vy,kmagiaodstien*(max(0.0,mapa.h-8*LOD_POLOMER-mojavec.imag()))-15),-kmagiaodstien*(max(0.0,mojavec.imag()-8*LOD_POLOMER))+15);
  konRych=Vec(vx,vy);

  //double force = INF;
  //vy += (mapa.h / 2 < mojavec.imag()) ? force / cube(mojavec.imag()+1) : -force / cube(mojavec.imag() - mapa.h / 2 + 1) ;
  //vx += (mapa.w / 2 < mojavec.real()) ? force / cube(mojavec.real()+1) : -force / cube(mojavec.real() - mapa.w / 2 + 1);

  //konRych=Vec(vx,vy);

  //konRych += Vec(1, 0) * (6000000 / square(abs(mojavec.real()) + 1));
  //konRych += Vec(-1, 0) * (6000000 / square(abs(mapa.w - mojavec.real()) + 1));
  //konRych += Vec(0, 1) * (6000000 / square(abs(mojavec.imag()) + 1));
  //konRych += Vec(0, -1) * (6000000 / square(abs(mapa.h - mojavec.imag()) + 1));

  konRych-=vecovat(ja.obj.rychlost);
  double v=abs(konRych);
  v=max(v,-200.0);
  v=min(v,200.0);
  konRych=norm2(konRych)*v;
  prikaz.acc = odvecovat(konRych);

  //STRIELAJ

  double minscore=1E18;

  for(Hrac h:stav.hraci)
  {
      if(!h.zije()) continue;
      if(vecovat(h.obj.pozicia)==mojavec) continue;
      if((abs(vecovat(h.obj.pozicia) - mojavec)+ h.obj.zivoty) < minscore )
      {
          minscore=(abs(vecovat(h.obj.pozicia) - mojavec)+ h.obj.zivoty);
          prikaz.ciel=odvecovat(vecovat(h.obj.pozicia)-mojavec+vecovat(h.obj.rychlost)-vecovat(ja.obj.rychlost));
      }
  }
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
