#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

#include "common.h"

static int volne_id=0;
static int volny_hrac=0;

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

Bod Bod::operator*(const Bod& B) const { // A a B zacinaju v rovn. bode, kolmica z B na A tvori 2. bod vysl. vektoru
  double skalarnySucin = x*B.x + y*B.y;
  double dlzka = skalarnySucin / B.dist();
  return B*(dlzka / B.dist());
}
double Bod::operator/(const Bod& B) const { // kolkonasobok B tvori so mnou pravouhly trojuholnik ?
  Bod temp = (*this)*B;
  if (temp.x == 0.0) {
    return temp.y/B.y;
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

bool FyzikalnyObjekt::zije () {
  return zivoty > 0;
}
bool FyzikalnyObjekt::neznicitelny () {
  return stit>0 || zivoty>INDESTRUCTIBLE;
}


Vec::Vec (const Bod& poz,const int& t,const int& ammo) : typ(t), naboje(ammo)
{
  obj= FyzikalnyObjekt(VEC,-1, poz, Bod(), VEC_POLOMER, VEC_KOLIZNY_LV, VEC_SILA, VEC_ZIVOTY);
}
Vec::Vec () {}

bool Vec::zije () {
  return obj.zije();
}


Hrac::Hrac (const Bod& poz) : skore(0.0), zbrane(DRUHOV_ZBRANI), cooldown(0), veci(DRUHOV_VECI)
{
  obj=FyzikalnyObjekt(LOD,volny_hrac, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_SILA, LOD_ZIVOTY);
  volny_hrac++;
}
Hrac::Hrac () {}

bool Hrac::zije () {
  return obj.zije();
}


Prikaz::Prikaz () : acc(Bod()), ciel(Bod()), pal(-1), pouzi(DRUHOV_VECI) {}


Mapa::Mapa (const double& sirka,const double& vyska) :
  w(sirka), h(vyska) {}
  
Mapa::Mapa () : w(-1), h(-1) {}


Stav::Stav () {}
