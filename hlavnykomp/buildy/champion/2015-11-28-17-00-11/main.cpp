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

int korist, citlivost, rage,pocetnazive=0;
pair<int,int> P;
double r1;
int x=-1;
// int r1=0;
 
Bod sprint(Bod k){
  return (k - stav.hraci[0].obj.pozicia)*0.4;
  
}

int najblizsi(){
  int a;
  for(int i=1; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0){
      a=i;
      break;
    }
  for(int i=a; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0 && (stav.hraci[i].obj.pozicia-stav.hraci[0].obj.pozicia).dist()<(stav.hraci[a].obj.pozicia-stav.hraci[0].obj.pozicia).dist())
      a=i;
  return a;
}
bool mimomapy(Bod A)
{
  if(A.x<0 || A.y<0 || A.x>=mapa.w || A.y>=mapa.h)
    return false;
  return true;
}
Bod bachastena()
{
    Bod X=Bod(stav.hraci[0].obj.rychlost.x*stav.hraci[0].obj.rychlost.x,stav.hraci[0].obj.rychlost.y*stav.hraci[0].obj.rychlost.y)*(-2);
    Bod Y=Bod(-stav.hraci[0].obj.rychlost.y,stav.hraci[0].obj.rychlost.x);
    Bod Z=(stav.hraci[0].obj.pozicia+stav.hraci[0].obj.rychlost)*2;
  if(mimomapy(Z))
    return X+Y;
  else 
    return sprint(stav.hraci[korist].obj.pozicia);
}
Bod pozorobj()
{
  P={-1,-1};
  for(int j=0;j<5;j++){
    if(j==4)
      continue;

    for(int i=0;i<stav.obj[j].size();i++){
      if(P.first==-1){
        if(stav.obj[j][i].zivoty>0)
          P={j,i};
        continue;
      }
      if((stav.obj[j][i].pozicia-stav.hraci[0].obj.pozicia).dist()-stav.obj[j][i].polomer<(stav.obj[P.first][P.second].pozicia-stav.hraci[0].obj.pozicia).dist()-stav.obj[P.first][P.second].polomer && stav.obj[j][i].zivoty>0);
        P={j,i};
    }
  }
  if(P.first!=-1)
  {
    if(P.first==1)
      return stav.hraci[0].obj.rychlost*(-1);
    else 
      return stav.obj[P.first][P.second].rychlost;
  }
  else 
    return Bod(0,0);
}
Bod zlato()
{
  x=-1;
  for(int i=0;i<stav.obj[4].size();i++)
  {
    if(x==-1)
    {
      x=i;
      continue;
    }
    if((stav.obj[4][i].pozicia-stav.hraci[0].obj.pozicia).dist()<(stav.obj[4][x].pozicia-stav.hraci[0].obj.pozicia).dist())
      x=i;
  }
  if(x==-1)return Bod(0,0);
  Bod D=stav.hraci[0].obj.rychlost;
  Bod E=D-(stav.obj[4][x].pozicia-stav.hraci[0].obj.pozicia);
  if(D.dist()<200)return E*3;
  else 
  {
    if(D.dist()<100)return E*5;
    return E;
  }
}
int setrage(){
  if ((stav.hraci[najblizsi()].obj.pozicia-stav.hraci[0].obj.pozicia).dist()<citlivost)
    return 1;
  else
    return 0;
}
int najkripel(){
  int a;
  for(int i=1; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0){
      a=i;
      break;
    }
  for(int i=a; i<stav.hraci.size(); i++)
    if (stav.hraci[i].obj.zivoty>0 && (stav.hraci[i].obj.rychlost).dist()<(stav.hraci[a].obj.rychlost).dist())
      a=i;
  return a;
}

void zistiTah() {
  citlivost=600;
  korist=najblizsi();
  for(int i=0;i<stav.hraci.size();i++)
    if(stav.hraci[i].obj.zivoty>0)
      pocetnazive++;
  rage=setrage();
  
  if (rage==0 && stav.hraci[0].zasobnik<5)
    prikaz.ciel=Bod(0,0);
  //if (rage==0 && stav.hraci[0].zasobnik==5)
    prikaz.ciel=stav.hraci[korist].obj.pozicia-stav.hraci[0].obj.pozicia+stav.hraci[korist].obj.rychlost-stav.hraci[0].obj.rychlost;
  /*else{
    if (stav.hraci[0].obj.rychlost.dist()<10){
      int kripel=najkripel();
      if (stav.hraci[kripel].obj.rychlost.dist()<10)
        prikaz.ciel=stav.hraci[kripel].obj.pozicia-stav.hraci[0].obj.pozicia+stav.hraci[kripel].obj.rychlost-stav.hraci[0].obj.rychlost;
    }*/
  //}
  
  Bod odsteny=bachastena(),odobj=pozorobj(),zzz=zlato();
  prikaz.acc=odsteny*10+odobj+zzz*2;
  if(stav.hraci[0].obj.rychlost.dist()>100)
    prikaz.acc=prikaz.acc+(stav.hraci[0].obj.rychlost*(-2));
  if(stav.hraci[0].obj.rychlost.dist()>200)
    prikaz.acc=prikaz.acc+(stav.hraci[0].obj.rychlost*(-3));
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
