#include <cmath>

#include "common.h"

static int volne_id=0;
static int volny_hrac=0;

int zbranNaVystrel (int zbran) {
  switch (zbran) {
    case VEC_PUSKA: return VYSTREL_PUSKA;
    case VEC_BOMBA: return VYSTREL_BOMBA;
    case VEC_LASER: return VYSTREL_LASER;
  }
  return -INF;
}

int vystrelNaProj (int vystrel) {
  switch (vystrel) {
    case VYSTREL_PUSKA: return BROK;
    case VYSTREL_BOMBA: return BOMBA;
  }
  return -INF;
}

int vecNaPouzi (int vec) {
  switch (vec) {
    case VEC_URYCHLOVAC: return POUZI_URYCHLOVAC;
    case VEC_STIT: return POUZI_STIT;
    case VEC_LEKARNICKA: return POUZI_LEKARNICKA;
  }
  return -INF;
}

Bod::Bod() {
  this->x = 0;
  this->y = 0;
}

Bod::Bod(double x, double y) {
  this->x = x;
  this->y = y;
}

Bod Bod::operator+(Bod iny) const {
  return Bod(x + iny.x, y + iny.y);
}

Bod Bod::operator-(Bod iny) const {
  return Bod(x - iny.x, y - iny.y);
}

Bod Bod::operator*(double k) const {
  return Bod(x * k, y * k);
}

bool Bod::operator== (Bod iny) const {
  return x==iny.x && y==iny.y;
}

double Bod::dist() const {
  return sqrt(x*x + y*y);
}

double Bod::dist2() const {
  return x*x + y*y;
}

Bod Bod::operator*(Bod B) const { // A a B zacinaju v rovn. bode, kolmica z B na A tvori 2. bod vysl. vektoru
  double skalarnySucin = x*B.x + y*B.y;
  double dlzka = skalarnySucin / B.dist();
  return B*(dlzka / B.dist());
}

double Bod::operator/(Bod B) const { // kolkonasobok B tvori so mnou pravouhly trojuholnik ?
  Bod temp = (*this)*B;
  if (B.x == 0) {
    return temp.y/B.y;
  }
  if (B.y == 0) {
    return 0.0;
  }
  return temp.x/B.x;
}


FyzikalnyObjekt::FyzikalnyObjekt (int t,int own, Bod poz,Bod v,
  double r, int coll, double pow,double hp) :
  typ(t), owner(own),
  pozicia(poz), rychlost(v), polomer(r),
  koliznyLevel(coll),
  sila(pow), zivoty(hp), stit(0)
{
  id=volne_id;
  volne_id++;
}

FyzikalnyObjekt::FyzikalnyObjekt () {}

bool FyzikalnyObjekt::zije () const {
  return zivoty > 0;
}
bool FyzikalnyObjekt::neznicitelny () const {
  return stit>0 || zivoty>INDESTRUCTIBLE;
}
double FyzikalnyObjekt::obsah () const {
  return PII*polomer*polomer;
}

Vybuch::Vybuch () {}

Vybuch::Vybuch (int own,Bod kde,double r,double dmg,int f) :
  owner(own), pozicia(kde), polomer(r), sila(dmg), faza(f)
{
  id=volne_id;
  volne_id++;
}


Vec::Vec (Bod poz,int t,int ammo) : typ(t), naboje(ammo)
{
  obj= FyzikalnyObjekt(t,-1, poz, Bod(), VEC_POLOMER, VEC_KOLIZNY_LV, VEC_SILA, VEC_ZIVOTY);
}
Vec::Vec () {}

bool Vec::zije () const {
  return obj.zije();
}


Hrac::Hrac (Bod poz) : skore(0.0), cooldown(0), veci(DRUHOV_VECI,0)
{
  obj=FyzikalnyObjekt(LOD,volny_hrac, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_SILA, LOD_ZIVOTY);
  volny_hrac++;
  for (int i=0; i<DRUHOV_ZBRANI; i++) {
    zbrane.push_back(k_nabojovNaZac[i]);
  }
}
Hrac::Hrac () {}

bool Hrac::zije () const {
  return obj.zije();
}


Prikaz::Prikaz () : acc(Bod()), ciel(Bod()), pal(-1), pouzi(DRUHOV_VECI) {}


Mapa::Mapa (double sirka,double vyska) :
  w(sirka), h(vyska) {}
  
Mapa::Mapa () : w(-1), h(-1) {}


Stav::Stav () {}

int Stav::zivychHracov () const {
  int res=0;
  for (const Hrac& hrac : hraci) {
    res+= hrac.zije();
  }
  return res;
}
