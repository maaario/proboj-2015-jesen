
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
using namespace std;

#define INF 1023456789ll
#define EPS 0.0000000001
#define PII 3.14159265358979323846

#define ASTEROID  0
#define PLANETA   1
#define BOSS      2
#define STRELA    3
#define ZLATO     4
#define LOD       5

// bodovanie
#define SKORE_ZA_PREZITIE  50
const static double kSkoreZnic[]={1,INF,INF,0,0,50}; // ziskane body za znicenie daneho objektu

#define NORM_TYPOV 5

// parametre asteroidu
#define AST_MIN_R       7.0 // minimalny polomer asteroidu po rozpade
#define AST_KOLIZNY_LV  10
#define AST_SILA        0.05
#define AST_ZIV_RATE    1.0
#define AST_DROP_RATE   0.4
#define AST_ROZPAD_ACC  30.0

// parametre planet
#define PLANETA_KOLIZNY_LV  100
#define PLANETA_SILA        0.05
#define PLANETA_ZIV_RATE    INF

// parametre lode
#define LOD_POLOMER     10.0
#define LOD_KOLIZNY_LV  10
#define LOD_SILA        1.0
#define LOD_ZIVOTY      100.0
#define LOD_MAX_ACC     100.0

// parametre zlata
#define ZLATO_HODNOTA     1.0
#define ZLATO_POLOMER     6.0
#define ZLATO_KOLIZNY_LV  1
#define ZLATO_SILA        0.0
#define ZLATO_ZIVOTY      10.0

// parametre bossa
#define BOSS_POLOMER    20.0
#define BOSS_KOLIZNY_LV 100
#define BOSS_SILA       99999.9
#define BOSS_ZIVOTY     INF
#define BOSS_MAX_ACC    100.0
#define BOSS_PERIODA    30.0

// parametre sentinelu
#define SENTINEL_SILA   0.05

// parametre zbrane
#define ZASOBNIK          5
#define DODAVACIA_DOBA    2.0
#define COOLDOWN          0.3
#define STRELA_POLOMER    5.0
#define STRELA_KOLIZNY_LV 5
#define STRELA_SILA       0.1
#define STRELA_ZIVOTY     EPS
#define STRELA_RYCHLOST   200.0

// ine konstanty
#define DELTA_TIME 0.01
#define FRAME_TIME 0.04

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
  
  Bod pata(Bod B) const ; // A.pata(B) == spojnica (0,0) a paty z A na vektor B
  double operator/(Bod B) const ; // A/B == kolkonasobok B tvori spolu s A pravouhly trojuholnik?
};


struct FyzickyObjekt {
  int typ;
  int owner;
  int id;

  Bod pozicia;
  Bod rychlost;
  double polomer;
  double sila;
  double zivoty;

  int koliznyLevel;

  // kolizny_lv == Ak 2 objekty koliduju, tak
  // ak maju rovnaky kolizny_lv, tak sa obe odrazia
  // inak sa odrazi iba ten s nizsim koliznym_lv (ale viac)

  // sila == zhruba ake velke zranenie dostane objekt, ktory do mna vrazi
  // presne mnozstvo zranenia zavisi od toho, ake velke okamzite zrychlenie
  // ta zrazka sposobi

  // zivoty == ak su <=0, tak objekt umiera

  FyzickyObjekt (int t,int own, Bod poz,Bod v,double r, double pow,double hp, int koll) ;
  FyzickyObjekt () ;

  bool zije () const ;
  void pohni (double dt) ;
  void zrychli (Bod acc, double dt) ;
  void okamziteZrychli (Bod acc) ;
};

struct Hrac {
  FyzickyObjekt obj;
  int zasobnik;
  double zasobnikCooldown;
  double cooldown;
  double skore;

  // kazdy hrac ma zasobnik 5 striel
  // dobijacia doba je DODAVACIA_DOBA sekund (ale dobija sa len ked zasobnik nie je plny),
  // zost. cas je v premennej zasobnikCooldown
  // medzi dvoma strelami hraca je aspon COOLDOWN sekund (zostavajuci cas je v premennej cooldown)

  Hrac (Bod poz) ;
  Hrac () ;

  bool zije () const ;
};

struct Prikaz {
  Bod acc; // udava smer pohybu
  Bod ciel; // kam strielam
  // obe su relativne k pozicii lode
  // nastavenim na Bod() (prazdny konstruktor) == na (0,0)
  // neakcelerujem/ nestrielam

  Prikaz () ;
};

struct Mapa {
  double w,h;
  double casAst; // ako casto sa v mape vynaraju z okraja asteroidy
  double casBoss; // kedy najskor pride boss
  double astMinR, astMaxR; // minimalny a maximalny mozny polomer vynarajuceho sa asteroidu
  double astMinVel, astMaxVel; // min. a max. mozna rychlost vynarajuceho sa asteroidu
  vector<Bod> spawny; // mozne pociatocne pozicie hracov
  vector<FyzickyObjekt> objekty; // obsahuje vsetky objekty, ktore su na mape umiestnene na zaciatku

  Mapa (double sirka,double vyska) ;
  Mapa () ;
};

struct Stav {
  double cas; // ako dlho hra uz trva
  double casAst; // cas do prichodu dalsieho asteroidu z okraja mapy
  double casBoss; // cas do prichodu bossa
  vector<FyzickyObjekt> obj[NORM_TYPOV]; // obsahuje objekty daneho typu
  // typy su ASTEROID, PLANETA, BOSS, STRELA
  vector<Hrac> hraci; // obsahuje udaje a objekty (vesmirne lode) hracov

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

reflection(FyzickyObjekt);
  member(typ);
  member(owner);
  member(id);
  member(pozicia);
  member(rychlost);
  member(polomer);
  member(koliznyLevel);
  member(sila);
  member(zivoty);
end();

reflection(Hrac);
  member(obj);
  member(zasobnik);
  member(zasobnikCooldown);
  member(cooldown);
  member(skore);
end();

reflection(Prikaz);
  member(acc);
  member(ciel);
end();

reflection(Mapa);
  member(w);
  member(h);
  member(casAst);
  member(casBoss);
  member(astMinR);
  member(astMaxR);
  member(astMinVel);
  member(astMaxVel);
  member(spawny);
  member(objekty);
end();

reflection(Stav);
  member(cas);
  member(casAst);
  member(casBoss);
  member(obj[ASTEROID]);
  member(obj[PLANETA]);
  member(obj[BOSS]);
  member(obj[STRELA]);
  member(obj[ZLATO]);
  member(hraci);
end();

#endif
//*/
