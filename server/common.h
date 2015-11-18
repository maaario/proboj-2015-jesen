
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

#define POMALOST_CASU 10ll
#define MAX_ACCEL 1.0

//rozne typy objektov
#define TYP_LOD 0
#define TYP_ASTEROID 100
#define TYP_PLANETA 200
#define TYP_HVIEZDA 300

#define TYP_LASER 400
#define TYP_RAKETA 500
#define TYP_MINA 600

#define TYP_URYCHLOVAC 700
#define TYP_STIT 800
#define TYP_LEKARNICKA 900

//parametre lode
#define LOD_POLOMER 10.0
#define LOD_KOLIZNY_LV 10
#define LOD_NEBEZPECNOST 10.0
#define LOD_ZIVOTY 100.0

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

struct FyzikalnyObjekt {
  int typ;

  Bod pozicia;
  Bod rychlost;
  double polomer;

  int koliznyLevel;

  double tvrdost;
  double zivoty;

  FyzikalnyObjekt (const int& t, const Bod& poz,const Bod& v,const double& r,
  const double& coll, const double& hard,const double& hp) :
    typ(t),
    pozicia(poz), rychlost(v), polomer(r),
    koliznyLevel(coll),
    tvrdost(hard), zivoty(hp) {}

  FyzikalnyObjekt () {}

  bool zije () {
    return zivoty > 0;
  }
};

struct Vec {
  int typ;
  double zivotnost;

  Vec (const int& t,const double& ziv) : typ(t), zivotnost(ziv) {}
  Vec () {}

  bool zije () {
    return zivotnost > 0;
  }
};

struct Hrac {
  FyzikalnyObjekt obj;
  double skore;
  vector<Vec> zbrane;
  vector<Vec> veci;

  Hrac (const Bod& poz) {
    obj=FyzikalnyObjekt(TYP_LOD, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_NEBEZPECNOST, LOD_ZIVOTY);
    skore= 0.0;
  }
  Hrac () {}
};

struct Prikaz {
  Bod acc, ciel;
  int pal;
  vector<int> pouzi;

  Prikaz () : acc(Bod()), ciel(Bod()), pal(-1) {}
};

struct Mapa {
  double w,h;
  double casDoChrchliaka;
  vector<Bod> spawny;
  vector<FyzikalnyObjekt> objekty;

  Mapa (const double& sirka,const double& vyska,const double& t) :
  w(sirka), h(vyska), casDoChrchliaka(t) {}

  Mapa () {}
};

struct Stav {
  double cas;
  vector<FyzikalnyObjekt> objekty;
  vector<Hrac> hraci;

  Stav () : cas(0.0) {}
};

#endif

#ifdef reflection
// tieto udaje pouziva marshal.cpp aby vedel ako tie struktury ukladat a nacitavat

reflection(Bod);
  member(x);
  member(y);
end();

reflection(FyzikalnyObjekt);
  member(typ);
  member(pozicia);
  member(rychlost);
  member(polomer);
  member(koliznyLevel);
  member(tvrdost);
  member(zivoty);
end();

reflection(Vec);
  member(typ);
  member(zivotnost);
end();

reflection(Hrac);
  member(obj);
  member(skore);
  member(zbrane);
  member(veci);
end();

reflection(Prikaz);
  member(acc);
  member(ciel);
  member(pal);
  member(pouzi);
end();

reflection(Mapa);
  member(w);
  member(h);
  member(casDoChrchliaka);
  member(spawny);
  member(objekty);
end();

reflection(Stav);
  member(cas);
  member(objekty);
  member(hraci);
end();

#endif
//*/
