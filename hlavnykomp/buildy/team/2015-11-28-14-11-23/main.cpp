#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <cmath>
#include <cstdlib>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;
FyzickyObjekt ja;

typedef long long ll;

Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia) {
  Bod rozdielPozicie = cielovaPozicia - pozicia;
  Bod cielovaRychlost = rozdielPozicie * 0.5;

  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
  double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
  if (pomerKMojejMaximalnejRychlosti > 1.0) {
    cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
  }

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  return cielovaAkceleracia;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
double angle_deg = 0;

double deg_to_rad(double deg)
{
  double res = deg/(180/3.141519);
  return res;
}

bool fire = false;

Bod get_random_point()
{
  ll x_max = mapa.h;
  ll y_max = mapa.w;
  ll x = rand() % x_max;
  ll y = rand() % y_max;
  x = rand() % 2 == 0 ? x : -x;
  y = rand() % 2 == 0 ? y : -y;
  return Bod(x, y);
}

int najblizsi_hrac(){
  double minimalna_vzdialenost = 2000;
  minimalna_vzdialenost *= minimalna_vzdialenost;
  double best[2] = {10000000, 0};
  for (int i = 1; i < stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) continue;
    FyzickyObjekt protihrac = stav.hraci[i].obj;
    double vzdielnost = (ja.pozicia.x - protihrac.pozicia.x) * (ja.pozicia.x - protihrac.pozicia.x) + (ja.pozicia.y - protihrac.pozicia.y) * (ja.pozicia.y - protihrac.pozicia.y);
    if (vzdielnost <= minimalna_vzdialenost)
      if (vzdielnost < best[0]) {
        best[0] = vzdielnost;
        best[1] = i;
      }
  }

  return best[1];
}

void zistiTah() {
  ja = stav.hraci[0].obj;
  /*
  angle_deg = (angle_deg-1);
  double angle_rad = deg_to_rad(angle_deg);
  long long radius = 40;
  double x_move = cos(angle_rad) * radius;
  double y_move = sin(angle_rad) * radius;
  fprintf(stderr, "x_move %lf y_move %lf\n", x_move, y_move);
  */

  int zamiereny_hrac = najblizsi_hrac();
  if (zamiereny_hrac)
    prikaz.ciel = stav.hraci[zamiereny_hrac].obj.pozicia - ja.pozicia;
  else
    prikaz.ciel = Bod(0, 0);

  prikaz.acc = Bod(0, 0);
}

int main() {
  // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

  unsigned seed = time(NULL) * getpid();
  srand(seed);

  nacitaj(cin,mapa);
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);

  while (cin.good()) {
    nacitaj(cin,stav);

    zistiTah();

    uloz(cout,prikaz);
    cout << endl;
  }
}
