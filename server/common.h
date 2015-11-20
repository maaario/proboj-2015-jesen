
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

#define INF 1023456789ll
#define INDESTRUCTIBLE 987654321ll

#define STAV_TYPOV 5
#define MAPA_TYPOV 3

#define ASTEROID 0
#define PLANETA 1
#define HVIEZDA 2
#define BOSS 3
#define PROJEKTIL 4
#define BROK 5
#define BOMBA 6
#define VEC 7
#define LOD 8

#define DRUHOV_ZBRANI 3
#define ZBRAN_PUSKA 0
#define ZBRAN_LASER 1
#define ZBRAN_BOMBA 2

#define DRUHOV_VECI 3
#define VEC_URYCHLOVAC 0
#define VEC_STIT 1
#define VEC_LEKARNICKA 2

//parametre lode
#define LOD_POLOMER 10.0
#define LOD_KOLIZNY_LV 10
#define LOD_SILA 10.0
#define LOD_ZIVOTY 100.0

//parametre sentinelu
#define SENTINEL_POLOMER 10.0
#define SENTINEL_SILA 10.0

//parametre veci
#define VEC_POLOMER 10.0
#define VEC_KOLIZNY_LV 5
#define VEC_SILA 10.0
#define VEC_ZIVOTY 100.0

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

  Bod operator*(const Bod& B) const { // A a B zacinaju v rovn. bode, kolmica z B na A tvori 2. bod vysl. vektoru
    double skalarnySucin = x*B.x + y*B.y;
    double dlzka = skalarnySucin / B.dist();
    return B*(dlzka / B.dist());
  }
  double operator/(const Bod& B) const { // kolkonasobok B tvori so mnou pravouhly trojuholnik ?
    Bod temp = (*this)*B;
    if (temp.x == 0.0) {
      return temp.y/B.y;
    }
    return temp.x/B.x;
  }
};

struct FyzikalnyObjekt {
  int typ;

  Bod pozicia;
  Bod rychlost;
  double polomer;

  int koliznyLevel;

  double sila;
  double zivoty;
  int stit;

  FyzikalnyObjekt (const int& t, const Bod& poz,const Bod& v,const double& r,
  const int& coll, const double& pow,const double& hp) :
    typ(t),
    pozicia(poz), rychlost(v), polomer(r),
    koliznyLevel(coll),
    sila(pow), zivoty(hp), stit(0) {}

  FyzikalnyObjekt () {}

  bool zije () {
    return zivoty > 0;
  }
  bool neznicitelny () {
    return stit>0 || zivoty>INDESTRUCTIBLE;
  }
};

struct Vec {
  FyzikalnyObjekt obj;
  int typ;
  int naboje;

  Vec (const Bod& poz,const int& t,const int& ammo) : typ(t), naboje(ammo)
  {
    obj= FyzikalnyObjekt(VEC, poz, Bod(), VEC_POLOMER, VEC_KOLIZNY_LV, VEC_SILA, VEC_ZIVOTY);
  }
  Vec () {}

  bool zije () {
    return obj.zije();
  }
};

struct Hrac {
  FyzikalnyObjekt obj;
  double skore;

  vector<int> zbrane;
  int cooldown;
  vector<int> veci;

  Hrac (const Bod& poz) : skore(0.0), zbrane(DRUHOV_ZBRANI), cooldown(0), veci(DRUHOV_VECI)
  {
    obj=FyzikalnyObjekt(LOD, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_SILA, LOD_ZIVOTY);
  }
  Hrac () {}

  bool zije () {
    return obj.zije();
  }
};

struct Prikaz {
  Bod acc;

  Bod ciel;
  int pal;

  vector<int> pouzi;

  Prikaz () : acc(Bod()), ciel(Bod()), pal(-1), pouzi(DRUHOV_VECI) {}
};

struct Mapa { //TODO: popis spawnovania asteroidov
  double w,h;
  vector<int> casBoss;
  vector<Bod> spawny;
  vector<FyzikalnyObjekt> objekty;
  vector<Vec> veci;

  Mapa (const double& sirka,const double& vyska) :
    w(sirka), h(vyska) {}

  Mapa () : w(-1), h(-1) {}
};

struct Stav {
  int cas;
  vector<FyzikalnyObjekt> obj[STAV_TYPOV];
  /*
  vector<FyzikalnyObjekt> asteroidy;
  vector<FyzikalnyObjekt> planety;
  vector<FyzikalnyObjekt> hviezdy;
  vector<FyzikalnyObjekt> bossovia;
  vector<FyzikalnyObjekt> projektily;
  */
  vector<Vec> veci;
  vector<Hrac> hraci;

  Stav () {}
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
  member(sila);
  member(zivoty);
  member(stit);
end();

reflection(Vec);
  member(obj);
  member(typ);
  member(naboje);
end();

reflection(Hrac);
  member(obj);
  member(skore);
  member(zbrane);
  member(cooldown);
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
  member(casBoss);
  member(spawny);
  member(objekty);
  member(veci);
end();

reflection(Stav);
  member(cas);
  member(obj[ASTEROID]);
  member(obj[PLANETA]);
  member(obj[HVIEZDA]);
  member(obj[BOSS]);
  member(obj[PROJEKTIL]);
  member(veci);
  member(hraci);
end();

#endif
//*/
