
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

#define POMALOST_CASU 1000ll
#define MAX_ACCEL 1.0

struct Bod {
  double x, y;

  Bod() {
    this->x = 0;
    this->y = 0;
  }

  Bod(double x, double y) {
    this->x = x;
    this->y = y;
  }

  Bod operator+(const Bod &iny) const {
    return Bod(x + iny.x, y + iny.y);
  }

  Bod operator-(const Bod &iny) const {
    return Bod(x - iny.x, y - iny.y);
  }

  Bod operator*(const double &k) const {
    return Bod(x * k, y * k);
  }

  double dist() const {
    return sqrt(x*x + y*y);
  }
};

struct Spawn {
  Bod pozicia;

  Spawn() {}
  Spawn(double x, double y) {
    pozicia.x = x;
    pozicia.y = y;
  }
};

struct Hrac {
  Bod pozicia, rychlost;

  Hrac () {}
  Hrac(double x, double y, double v_x, double v_y) {
    pozicia.x = x;
    pozicia.y = y;
    rychlost.x = v_x;
    rychlost.y = v_y;
  }
};

struct Prikaz {
  Bod akceleracia;

  Prikaz() {}
  Prikaz(Bod B) {
    akceleracia=B;
  }
  Prikaz(double a_x, double a_y) {
    akceleracia= Bod(a_x, a_y);
  }
};

struct Planeta {
  Bod pozicia;
  double polomer;

  Planeta() {}
  Planeta(double x, double y, double polomer) {
    pozicia.x = x;
    pozicia.y = y;
    this->polomer = polomer;
  }
};

struct Mapa { //obsahuje staticke objekty
  vector<Spawn> spawny;
  vector<Planeta> planety;
};

struct Stav { //dynamicke objekty
  long long time;
  vector<Hrac> hraci;
  
  void operator= (const Stav S) {
    time = S.time;
    hraci.clear();
    for (Hrac hrac : S.hraci) {
      hraci.push_back(hrac);
    }
  }
};

#endif

#ifdef reflection
// tieto udaje pouziva marshal.cpp aby vedel ako tie struktury ukladat a nacitavat

reflection(Bod);
  member(x);
  member(y);
end();

reflection(Spawn);
  member(pozicia);
end();

reflection(Hrac);
  member(pozicia);
  member(rychlost);
end();

reflection(Prikaz);
  member(akceleracia);
end();

reflection(Planeta);
  member(pozicia);
  member(polomer);
end();

reflection(Mapa);
  member(spawny);
  member(planety);
end();

reflection(Stav);
  member(time);
  member(hraci);
end();

#endif
