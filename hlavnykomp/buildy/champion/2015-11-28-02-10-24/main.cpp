#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

//#define For(i,n) for (int i=0; i<n; i++)

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

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

/*pair<Bod,Bod> synch(int s){
   
  return {stav.hraci[s].obj.pozicia, stav.hraci[s].obj.rychlost-stav.hraci[0].rychlost};
}*/

int korist, citlivost, rage;

Bod sprint(Bod k){
  return (k - stav.hraci[0].obj.pozicia*10);
  
}
Bod bachastena(){
  Bod kv= Bod(-stav.hraci[0].obj.rychlost.y,stav.hraci[0].obj.rychlost.x),
      //nb= stav.hraci[0].obj.pozicia+kv,
      np= stav.hraci[0].obj.pozicia+stav.hraci[0].obj.rychlost;
  //if (nb.x>mapa.w || nb.x<0 || nb.y>mapa.h || nb.y>0)
  if ((np+kv).x>0 && (np+kv).y>0 && (np+kv).x<mapa.w && (np+kv).y<mapa.h)
    return stav.hraci[0].obj.pozicia+kv*200;
  return stav.hraci[0].obj.pozicia-kv*200;
}
Bod strela(){
  int n;
}
int najblizsi(){
  int a;
  for(int i=1; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0){
      a=i;
      break;
    }
  for(int i=1; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0)
      if ((stav.hraci[0].obj.pozicia-stav.hraci[a].obj.pozicia).dist()>(stav.hraci[0].obj.pozicia-stav.hraci[i].obj.pozicia).dist())
	a=i;
  return a;
}
int setrage (){
    if ((stav.hraci[0].obj.pozicia-stav.hraci[korist].obj.pozicia).dist()<citlivost)
      return 1;
    else
      return 0;
}

void zistiTah() {
  korist=najblizsi();
  rage=setrage();
  citlivost=500;
  //Bod r1=stav.hraci[0].obj.rychlost-stav.hraci[korist].obj.rychlost;
  
  prikaz.acc = sprint(stav.hraci[korist].obj.pozicia);   // akceleracia
  
  Bod np= Bod(stav.hraci[0].obj.pozicia+stav.hraci[0].obj.rychlost);
  if (np.x>mapa.w || np.x<0 || np.y>mapa.h || np.y>0)
  //if(mapa.w/20>stav.hraci[0].obj.pozicia.x || mapa.h/20>stav.hraci[0].obj.pozicia.y || mapa.w*19/20<stav.hraci[0].obj.pozicia.x || mapa.h*19/20<stav.hraci[0].obj.pozicia.y)
    prikaz.acc=bachastena();
  
  if ((rage==1)||((rage==0)&&(stav.hraci[0].zasobnik==5)))
    prikaz.ciel = (stav.hraci[korist].obj.pozicia-stav.hraci[0].obj.pozicia)+(stav.hraci[korist].obj.rychlost-stav.hraci[0].obj.rychlost);    // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
  else
    prikaz.ciel=Bod(0,0);
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
