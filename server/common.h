
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
using namespace std;

#define INF 1023456789ll
#define PII 3.14159265358979323846

#define ASTEROID 0
#define PLANETA 1
#define HVIEZDA 2
#define BOSS 3
#define PROJ_BEGIN 4
#define BROK 4
#define BOMBA 5
#define VEC_BEGIN 6
#define VEC_PUSKA 6
#define VEC_BOMBA 7
#define VEC_LASER 8
#define VEC_URYCHLOVAC 9
#define VEC_STIT 10
#define VEC_LEKARNICKA 11
#define LOD 12

#define STAV_TYPOV 5
const static int kStavTypy[STAV_TYPOV]=
  {ASTEROID,PLANETA,HVIEZDA,BOSS,PROJ_BEGIN};
#define MAPA_TYPOV 4
// parametre tykajuce sa bodovania
#define VSETKO_TYPOV 13
const static double kBodyZnic[VSETKO_TYPOV] =
  {10,500,INF,5000,  0,0,  0,0,0,0,0,0, 5000};


#define DRUHOV_ZBRANI 3
#define DRUHOV_PROJ 2
#define VYSTREL_PUSKA 0
#define VYSTREL_BOMBA 1
#define VYSTREL_LASER 2

#define DRUHOV_VECI 3
#define POUZI_URYCHLOVAC 0
#define POUZI_STIT 1
#define POUZI_LEKARNICKA 2

// parametre asteroidu
#define AST_MIN_R 5.0
#define AST_MAX_R 50.0
#define AST_KOLIZNY_LV 10
#define AST_SILA 10.0
#define AST_ZIV_RATE 10.0
#define AST_DROP_RATE 0.02
#define AST_ROZPAD_ACC 0.5

// parametre lode
#define LOD_POLOMER 10.0
#define LOD_KOLIZNY_LV 10
#define LOD_SILA 10.0
#define LOD_ZIVOTY 100.0
#define LOD_MAX_ACC 0.005

// parametre veci
#define VEC_POLOMER 10.0
#define VEC_KOLIZNY_LV 5
#define VEC_SILA 0.0
#define VEC_ZIVOTY 100.0
const static int kV_nabojov[DRUHOV_ZBRANI+DRUHOV_VECI]=
  {20,2,5, 2,2,2};

// parametre bossa
#define BOSS_POLOMER 30.0
#define BOSS_KOLIZNY_LV 100
#define BOSS_SILA 1023456789ll
#define BOSS_ZIVOTY 1023456789ll
#define BOSS_MAX_ACC 0.0015

// parametre sentinelu
#define SENTINEL_SILA 10.0

// parametre PROJ_BEGINov
#define BUM_POLOMER 30.0
#define BUM_SILA 5.0
#define BUM_TRVANIE 50
const static double kZ_polomer[DRUHOV_PROJ]= {5.0, 6.0};
const static int kZ_koliznyLv[DRUHOV_PROJ]= {0, 0};
const static double kZ_sila[DRUHOV_PROJ]= {10.0, 0.0};
const static double kZ_zivoty[DRUHOV_PROJ]= {0.0001, 0.0001};
const static double kZ_rychlost[DRUHOV_PROJ]= {1.0, 0.4};

// parametre ostatnych nebezpecnych veci
#define LASER_SILA 0.5

// ine parametre
#define INDESTRUCTIBLE 987654321ll
#define COOLDOWN 25
#define BROKOV_NA_ZACIATKU 20
#define AST_MAX_V 1.0


int zbranNaVystrel (int zbran) ;

int vystrelNaProj (int vystrel) ;

int vecNaPouzi (int vec) ;

struct Bod {
  double x, y;

  Bod() ;

  Bod(double x, double y) ;

  Bod operator+(Bod iny) const ;

  Bod operator-(Bod iny) const ;

  Bod operator*(double k) const ;

  bool operator== (Bod iny) const ;

  double dist() const ;
  double dist2() const ;
  Bod operator*(Bod B) const ;
  double operator/(Bod B) const ;
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

  FyzikalnyObjekt (int t,int own, Bod poz,Bod v,double r, int coll,double pow,double hp) ;

  FyzikalnyObjekt () ;

  bool zije () const ;
  bool neznicitelny () const ;
  double obsah () const ;
};

struct Vybuch {
  int owner;
  int id;
  Bod pozicia;
  double polomer;
  double sila;
  int faza;

  Vybuch () ;
  Vybuch (int own,Bod kde,double r,double dmg,int f) ;
};

struct Vec {
  FyzikalnyObjekt obj;
  int typ;
  int naboje;

  Vec (Bod poz,int t,int ammo) ;
  Vec () ;

  bool zije () const ;
};

struct Hrac {
  FyzikalnyObjekt obj;
  double skore;

  vector<int> zbrane;
  int cooldown;
  vector<int> veci;

  Hrac (Bod poz) ;
  Hrac () ;

  bool zije () const ;
};

struct Prikaz {
  Bod acc; // udava smer pohybu (je relativny)

  Bod ciel; // udava poziciu ciela (je ABSOLUTNY)
  int pal;

  vector<int> pouzi;

  Prikaz () ;
};

struct Mapa { //TODO: popis spawnovania asteroidov
  double w,h;
  int casBoss;
  int casAst;
  vector<Bod> spawny;
  vector<FyzikalnyObjekt> objekty;
  vector<Vec> veci;

  Mapa (double sirka,double vyska) ;

  Mapa () ;
};

struct Stav {
  int cas;
  vector<FyzikalnyObjekt> obj[STAV_TYPOV];
  vector<Vybuch> vybuchy;
  vector<Vec> veci;
  vector<Hrac> hraci;

  Stav () ;

  int zivychHracov () const ;
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

reflection(Vybuch);
  member(owner);
  member(id);
  member(pozicia);
  member(polomer);
  member(sila);
  member(faza);
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
  member(casAst);
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
  member(obj[PROJ_BEGIN]);
  member(vybuchy);
  member(veci);
  member(hraci);
end();

#endif
//*/
