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
  if (cielovaRychlost.dist() > EPS) {
    double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
    if (pomerKMojejMaximalnejRychlosti > 1.0) {
      cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
    }
  }

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  
  return cielovaAkceleracia;
}

double toRad(double angle){
  return (angle-180)*PII / 90;
}

double priamaVzd(Bod b, Bod c = stav.hraci[0].obj.pozicia){
  return sqrt(pow(c.x - b.x , 2) + pow(c.y - b.y , 2));
}

double alfa(Bod a, Bod b, Bod c){
  return acos((pow(priamaVzd(a, b), 2) + pow(priamaVzd(b, c), 2) - pow(priamaVzd(a, c), 2)) / (priamaVzd(a, b)*priamaVzd(b, c)*2));
}

bool convex(Bod a, Bod b, Bod c, Bod d){
  if(alfa(a, b, c) <= 2*PII + EPS && alfa(b, c, d) <= 2*PII + EPS && alfa(c, d, a) <= 2*PII + EPS && alfa(d, a, b) <= 2*PII + EPS) return 1;
  return 0;
}

int trafi(double t, FyzickyObjekt o){
  Bod v = o.rychlost - stav.hraci[0].obj.rychlost;
  Bod d = Bod(v.x * (t+COOLDOWN), v.x * (t+COOLDOWN));
  Bod poz = d + o.pozicia;
  Bod newPoz = stav.hraci[0].obj.rychlost * COOLDOWN + stav.hraci[0].obj.pozicia;
  double vzd = priamaVzd(poz, newPoz);
  double time = vzd / STRELA_RYCHLOST;
  if(time > t+EPS) return -1;
  else if(time < t - EPS) return 1;
  else return 0;
}

double prekazkaVCeste(FyzickyObjekt o){
  double a = (o.pozicia.y - stav.hraci[0].obj.rychlost.y) / (o.pozicia.x - stav.hraci[0].obj.rychlost.x);
  if(abs(stav.hraci[0].obj.pozicia.x * a - stav.hraci[0].obj.pozicia.y) <= LOD_POLOMER){
    return (priamaVzd(stav.hraci[0].obj.pozicia, o.pozicia)) / priamaVzd(o.rychlost - stav.hraci[0].obj.rychlost, Bod());    
  }
  return -1;
}

double alpha(FyzickyObjekt o, double z, double k){
  if(toRad(z-k) < EPS){
    return (z);
  }
  if(trafi(z+(k-z)/2, o)==-1) return alpha(o, z, z+(k-z)/2);
  else if(trafi(z+(k-z)/2, o)==1) return alpha(o, z+(k-z)/2, k);
  else return (z+(k-z)/2);  
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

bool hybeSa = false;
int faza = 0;

void kriza(){
  fprintf(stderr, "KRIZA!\n");
  prikaz.ciel = Bod();
  double dxx = -stav.hraci[0].obj.rychlost.x, dyy = stav.hraci[0].obj.rychlost.y;
  faza = 1;
  prikaz.acc = prikaz.acc + Bod(dxx, dyy);
  prikaz.ciel = Bod();
  hybeSa = true;
}

bool niecoVCeste(){
  //CI sa zrazi s objectami
  int zrazitelne[] = {STRELA, ASTEROID, PLANETA};
  FOR(i, 3){
    FOR(j, stav.obj[zrazitelne[i]].size()){
      if(prekazkaVCeste(stav.obj[zrazitelne[i]][j]) < 0.2 && prekazkaVCeste(stav.obj[zrazitelne[i]][j])!=-1){
	cerr << i << " " << j << endl;
	return 1;
      }
    }
  }
  
  //CI sa zrazi so stenou 
  Bod pos = stav.hraci[0].obj.rychlost * 0.2 + stav.hraci[0].obj.pozicia;
  if(pos.x <= 0 || pos.x >= mapa.w || pos.y <= 0 || pos.y >= mapa.h) return 1;
  
  //CI sa zrazi s hracmi
  
  FOR(i, stav.hraci.size()){
    if(i!=0){
      if(prekazkaVCeste(stav.hraci[i].obj) < 0.2 && prekazkaVCeste(stav.hraci[i].obj)!=-1) return 1;
    }
  }
  
  return 0;
}


char mode = 'N';
int fixID = -1;
Bod krizaBef = Bod();
Bod fixBod = Bod();

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  //MODE
  
  if(stav.obj[BOSS].size() > 0) mode = 'B';
  
  if(mode == 'B'){
    prikaz.acc = Bod(rand()%400 - 200, rand()%400 - 200);
    if(stav.hraci[0].zasobnik != 0){
      Bod ciel = najdiCiel();
      prikaz.ciel = ciel - stav.hraci[0].obj.pozicia;
    }
    return;
  }
  
  //normalny stav
  if(faza == 1){
    if(fixID != -1 && fixID < stav.obj[ZLATO].size()){
      prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, stav.obj[ZLATO][fixID].pozicia)+Bod(-100, 0);
      prikaz.ciel=Bod();
      if(priamaVzd(prikaz.acc, Bod()) > EPS * 10);
      else fixID = -1;
    }
    else prikaz.acc = Bod(-100, 0);
    prikaz.ciel = Bod();
    faza = 2;
    return;
  }
  if(faza == 2){
    if(fixID != -1 && fixID < stav.obj[ZLATO].size()){
      prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, stav.obj[ZLATO][fixID].pozicia)+Bod(-100, 0);
      prikaz.ciel=Bod();
      if(priamaVzd(prikaz.acc, Bod()) > EPS * 10);
      else fixID = -1;
    }
    else prikaz.acc = Bod(-100, 0);
    prikaz.ciel = Bod();
    faza = 0;
    return;
  }
  
  prikaz.acc = Bod();   // akceleracia
  if(fixID != -1 && fixID < stav.obj[ZLATO].size()){
    prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, stav.obj[ZLATO][fixID].pozicia);
    prikaz.ciel=Bod();
    if(priamaVzd(prikaz.acc, Bod()) > EPS * 10);
    else fixID = -1;
  }
  else if(stav.hraci[0].zasobnik == 0){
    int id=-1;
    double minVZD = INF;
    FOR(i, stav.obj[ZLATO].size()){
      if(priamaVzd(stav.obj[ZLATO][i].rychlost, Bod()) <= EPS * 100){
	if(minVZD > priamaVzd(stav.obj[ZLATO][i].pozicia)){
	  minVZD = priamaVzd(stav.obj[ZLATO][i].pozicia);
	  id = i;
	}
      }
    }
    if(id!=-1){
      fixID = id;
      prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, stav.obj[ZLATO][id].pozicia);
      prikaz.ciel = Bod();
    }
    else prikaz.acc = kontroler(stav.hraci[0].obj.pozicia, stav.hraci[0].obj.rychlost, stav.hraci[0].obj.pozicia+Bod(EPS, EPS));
    return;
  }
  
  if(niecoVCeste()) kriza();
  
  if(stav.hraci[0].zasobnik == 0) return; //tu detekovat ci sa nezrazi
  
  if(priamaVzd(stav.hraci[0].obj.rychlost, Bod()) > 1000.0){
    prikaz.acc = prikaz.acc - stav.hraci[0].obj.rychlost;
  }
  
  Bod ciel = najdiCiel();
  
  prikaz.ciel = ciel - stav.hraci[0].obj.pozicia;
}

int main() {
  // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).
  unsigned seed = time(NULL) * getpid();
  srand(seed);

  nacitaj(cin,mapa);
  fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);

  while (cin.good()) {
    nacitaj(cin,stav);

    if(!(stav.hraci[0].obj.rychlost == Bod())) hybeSa = false;
    /*if(!hybeSa)*/ zistiTah();

    uloz(cout,prikaz);
    cout << endl;
    DX = min(mapa.astMinR, LOD_POLOMER);
    //minAngle=max(mapa.w, mapa.h)
  }
}
