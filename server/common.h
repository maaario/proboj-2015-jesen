
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
using namespace std;

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
#define EXPLOZIA 9

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
#define LOD_MAX_ACC 0.001

//parametre sentinelu
#define SENTINEL_POLOMER 100.0
#define SENTINEL_SILA 10.0

//parametre veci
#define VEC_POLOMER 10.0
#define VEC_KOLIZNY_LV 5
#define VEC_SILA 10.0
#define VEC_ZIVOTY 100.0

struct Bod {
  double x, y;

  Bod() ;

  Bod(double x, double y) ;

  Bod operator+(const Bod &iny) const ;

  Bod operator-(const Bod &iny) const ;

  Bod operator*(const double &k) const ;

  bool operator== (const Bod& iny) const ;

  double dist() const ;
  Bod operator*(const Bod& B) const ;
  double operator/(const Bod& B) const ;
};

struct FyzikalnyObjekt {
  int typ;
  int owner;
  int id;

  Bod pozicia;
  Bod rychlost;
  double polomer;

  int koliznyLevel;

  double sila;
  double zivoty;
  int stit;

  FyzikalnyObjekt (const int& t,const int& own, const Bod& poz,const Bod& v,
    const double& r, const int& coll, const double& pow,const double& hp) ;

  FyzikalnyObjekt () ;

  bool zije () ;
  bool neznicitelny () ;
};

struct Vec {
  FyzikalnyObjekt obj;
  int typ;
  int naboje;

  Vec (const Bod& poz,const int& t,const int& ammo) ;
  Vec () ;

  bool zije () ;
};

struct Hrac {
  FyzikalnyObjekt obj;
  double skore;

  vector<int> zbrane;
  int cooldown;
  vector<int> veci;

  Hrac (const Bod& poz) ;
  Hrac () ;

  bool zije () ;
};

struct Prikaz {
  Bod acc;

  Bod ciel;
  int pal;

  vector<int> pouzi;

  Prikaz () ;
};

struct Mapa { //TODO: popis spawnovania asteroidov
  double w,h;
  vector<int> casBoss;
  vector<Bod> spawny;
  vector<FyzikalnyObjekt> objekty;
  vector<Vec> veci;

  Mapa (const double& sirka,const double& vyska) ;

  Mapa () ;
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

  Stav () ;
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
  member(owner);
  member(id);
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
