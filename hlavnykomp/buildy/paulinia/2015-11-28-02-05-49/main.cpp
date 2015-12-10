#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

#define FOR(i,n)	for(int i=0;i<n;i++)

Mapa mapa;
Stav stav; //hrac cislo 0
Prikaz prikaz;

double vx=0, vy=0, ax=0, ay=0;
int minAngle = 0;
double DX;

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
}

double toRad(double angle){
  return (angle-180)*PII / 90;
}

double priamaVzd(Bod b){
  return sqrt(pow(stav.hraci[0].obj.pozicia.x - b.x , 2) + pow(stav.hraci[0].obj.pozicia.y - b.y , 2));
}

int trafi(double t, FyzickyObjekt o){
  Bod d = Bod(o.rychlost.x * (t+COOLDOWN), o.rychlost.x * (t+COOLDOWN));
  Bod poz = d + o.pozicia;
  double vzd = priamaVzd(poz);
  double time = vzd / STRELA_RYCHLOST;
  if(time > t+EPS) return -1;
  else if(time < t - EPS) return 1;
  else return 0;
//   double dt = (o.pozicia.x - stav.hraci[0].obj.pozicia.x + o.pozicia.x * COOLDOWN ) / ( STRELA_RYCHLOST * cos( toRad(angle) ) ) -  (o.pozicia.x - stav.hraci[0].obj.pozicia.y + o.pozicia.y * COOLDOWN) / ( STRELA_RYCHLOST * sin( toRad(angle) ) );
//   if (abs(dt) <= EPS ){
//     return 1;
//   }
//   else if( dt > 0 )
}

bool prekazkaVCeste(){
  
}

double alpha(FyzickyObjekt o, double z, double k){
  if(toRad(z-k) < EPS){
    return toRad(z);
  }
  if(trafi(z+(k-z)/2, o)==-1) return alpha(o, z, z+(k-z)/2);
  else if(trafi(z+(k-z)/2, o)==1) return alpha(o, z+(k-z)/2, k);
  else return toRad(z+(k-z)/2);  
}

Bod najdiCiel(){
  //zatial verzia
  int id = rand()%stav.hraci.size();
  while(stav.hraci[id].zije()==0) id = rand()%stav.hraci.size();
  int time = alpha(stav.hraci[id].obj, 0, 10);
  Bod d = Bod(stav.hraci[id].obj.rychlost.x * (time+COOLDOWN), stav.hraci[id].obj.rychlost.x * (time+COOLDOWN));
  Bod poz = d + stav.hraci[id].obj.pozicia;
  return poz;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  prikaz.acc = Bod();   // akceleracia
  double uhol = rand()%360-180;
  double angle = PII * uhol / 90;
  Bod ciel = najdiCiel();
  
  prikaz.ciel = Bod(cos(angle), sin(angle));  // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
  prikaz.ciel = ciel - stav.hraci[0].obj.pozicia;
  if(rand()%4 == 1){
    
  }
  //
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
    DX = min(mapa.astMinR, LOD_POLOMER);
    //minAngle=max(mapa.w, mapa.h)
  }
}
