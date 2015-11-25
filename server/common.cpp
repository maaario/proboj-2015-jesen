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

Bod Bod::pata(Bod B) const { // ja a B zaciname v rovn. bode, kolmica z mna na B tvori 2. bod vysl. vektoru
  if (B==Bod()) {
    return Bod();
  }
  double skalarnySucin = x*B.x + y*B.y;
  double dlzka = skalarnySucin / B.dist();
  return B*(dlzka / B.dist());
}

double Bod::operator/(Bod B) const { // kolkonasobok B tvori so mnou pravouhly trojuholnik ?
  Bod temp = (*this).pata(B);
  if (B.x == 0) {
    return temp.y/B.y;
  }
  return temp.x/B.x;
}


FyzickyObjekt::FyzickyObjekt (int t,int own, Bod poz,Bod v,
  double r, int koll, double pow,double hp) :
  typ(t), owner(own),
  pozicia(poz), rychlost(v), polomer(r),
  koliznyLevel(koll),
  sila(pow), zivoty(hp)
{
  id=volne_id;
  volne_id++;
}

FyzickyObjekt::FyzickyObjekt () {}

bool FyzickyObjekt::zije () const {
  return zivoty > 0;
}
void FyzickyObjekt::pohni (double dt) {
  pozicia= pozicia + rychlost*dt;
}
void FyzickyObjekt::zrychli (Bod acc, double dt) {
  rychlost= rychlost + acc*dt;
}
void FyzickyObjekt::okamziteZrychli (Bod acc) {
  rychlost= rychlost + acc;
}

Hrac::Hrac (Bod poz) : zasobnik(ZASOBNIK), zasobnikCooldown(DODAVACIA_DOBA), cooldown(0), skore(0)
{
  obj=FyzickyObjekt(LOD,volny_hrac, poz, Bod(), LOD_POLOMER, LOD_KOLIZNY_LV, LOD_SILA, LOD_ZIVOTY);
  volny_hrac++;
}
Hrac::Hrac () {}

bool Hrac::zije () const {
  return obj.zije();
}


Prikaz::Prikaz () : acc(Bod()), ciel(Bod()) {}


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
