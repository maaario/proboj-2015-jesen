#include <cmath>

#include "common.h"

static int volne_id=0;
static int volny_hrac=0;

int zbran_na_proj (const int& typ_zbrane) {
  switch (typ_zbrane) {
    case ZBRAN_PUSKA: return BROK;
    case ZBRAN_BOMBA: return BOMBA;
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

Bod Bod::operator+(const Bod &iny) const {
  return Bod(x + iny.x, y + iny.y);
}

Bod Bod::operator-(const Bod &iny) const {
  return Bod(x - iny.x, y - iny.y);
}

Bod Bod::operator*(const double &k) const {
  return Bod(x * k, y * k);
}

bool Bod::operator== (const Bod& iny) const {
  return x==iny.x && y==iny.y;
}

double Bod::dist() const {
  return sqrt(x*x + y*y);
}

double Bod::dist2() const {
  return x*x + y*y;
}

Bod Bod::operator*(const Bod& B) const { // A a B zacinaju v rovn. bode, kolmica z B na A tvori 2. bod vysl. vektoru
  double skalarnySucin = x*B.x + y*B.y;
  double dlzka = skalarnySucin / B.dist();
  return B*(dlzka / B.dist());
}

double Bod::operator/(const Bod& B) const { // kolkonasobok B tvori so mnou pravouhly trojuholnik ?
  Bod temp = (*this)*B;
  if (B.x == 0) {
    return temp.y/B.y;
  }
  if (B.y == 0) {
    return 0.0;
  }
  return temp.x/B.x;
}


FyzikalnyObjekt::FyzikalnyObjekt (const int& t,const int& own, const Bod& poz,const Bod& v,
  const double& r, const int& coll, const double& pow,const double& hp) :
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


Vec::Vec (const Bod& poz,const int& t,const int& ammo) : typ(t), naboje(ammo)
{
  obj= FyzikalnyObjekt(VEC,-1, poz, Bod(), VEC_POLOMER, VEC_KOLIZNY_LV, VEC_SILA, VEC_ZIVOTY);
}
Vec::Vec () {}

bool Vec::zije () const {
  return obj.zije();
}


Hrac::Hrac (const Bod& poz) : skore(0.0), zbrane(DRUHOV_ZBRANI,0), cooldown(0), veci(DRUHOV_VECI,0)
{
  obj=FyzikalnyObjekt(LOD,volny_hrac, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_SILA, LOD_ZIVOTY);
  volny_hrac++;
  zbrane[ZBRAN_PUSKA]= BROKOV_NA_ZACIATKU;
}
Hrac::Hrac () {}

bool Hrac::zije () const {
  return obj.zije();
}


Prikaz::Prikaz () : acc(Bod()), ciel(Bod()), pal(-1), pouzi(DRUHOV_VECI) {}


Mapa::Mapa (const double& sirka,const double& vyska) :
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
