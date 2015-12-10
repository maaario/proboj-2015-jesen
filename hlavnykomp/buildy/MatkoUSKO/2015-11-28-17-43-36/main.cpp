#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

void uhni(Bod v);
bool mozem(Bod b);
void strielaj(FyzickyObjekt F);
Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia);


Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;
FyzickyObjekt boss;
FyzickyObjekt ja;
Bod MP;

struct Bod ciel;
// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void zistiTah() {
  ja = stav.hraci[0].obj;
  MP = stav.hraci[0].obj.pozicia; 

  vector<FyzickyObjekt*> objekty;
  vector<FyzickyObjekt*> goldy;
  zoznamObjekty(stav,objekty);
  vector<FyzickyObjekt*> super;
  int prec = -1;

  for (int i = 0; i < objekty.size(); i++) {
    if (objekty[i]->typ == BOSS){
      boss = *objekty[i];
    }else if((objekty[i] -> typ == LOD) && (objekty[i] -> id != ja.id) && objekty[i] -> zije()){
      super.push_back(objekty[i]);
    }else if(objekty[i] -> typ == ZLATO){
      goldy.push_back(objekty[i]);
    }
  }
  
  for(int i = 0; i < super.size(); i++){
    Bod vz = MP - super[i] -> pozicia;
    if(vz.dist() < 420){
      prec = i;
      break;
    }  
  }

  int mini = 1000000000;
  int minizlato = -1;
  for(int i = 0; i < goldy.size(); i++){
    if(mini < Bod((goldy[i] -> pozicia) - MP).dist()){
      minizlato = i;
      mini = Bod((goldy[i] -> pozicia) - MP).dist();
    }
  }
  
  
  Bod vectorvzdialenosti = MP - boss.pozicia;
  if(vectorvzdialenosti.dist() < 800){
    uhni(boss.rychlost);
  }else if(prec != -1){
    uhni(super[prec] -> rychlost);
    strielaj(*super[prec]);
  }else if(minizlato != -1){
    prikaz.acc = kontroler(MP, ja.rychlost, goldy[minizlato] -> pozicia);
  }else{
    prikaz.acc = Bod(0,0) - ja.rychlost;
  }

  
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

void uhni(Bod v){
  ciel.x = v.y;
  ciel.y = -v.x;
  if(!mozem(ciel)){
    ciel.x = -v.y;
    ciel.y = v.x; 
  }

  prikaz.acc = ciel;
}

bool mozem(Bod b){
  return ((b.x < mapa.w) && (b.x > 0) && (b.y < mapa.h) && (b.y > 0));
}

void strielaj(FyzickyObjekt F){
  prikaz.ciel = F.pozicia + F.rychlost - (ja.pozicia + ja.rychlost); 
}

Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia) {
  Bod rozdielPozicie = cielovaPozicia - pozicia;
  Bod cielovaRychlost = rozdielPozicie * 0.5;

  // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
  
  if(cielovaRychlost.dist() != 0){
    double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
    if (pomerKMojejMaximalnejRychlosti > 1.0) {
      cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
    }
  }

  Bod rozdielRychlosti = cielovaRychlost - rychlost;
  Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
  return cielovaAkceleracia;
}
