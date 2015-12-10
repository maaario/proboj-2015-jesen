#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

Bod strela;
Bod akcel;
Hrac ja;    

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

double vzdialenost(Bod a, Bod b)
{
    return ( sqrt( (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)) );
}

/*bool zrazima(FyzickyObjekt ja, FyzickyObjekt on)
{
    
    Bod jasmer = ja.rychlost;
    Bod onsmer = on.rychlost;
    Bod japos = ja.pozicia;
    Bod onpos = on.pozicia;
    4
    double jasx = jasmer.x;
    double jasy = jasmer.y;
    
    double onsx = onsmer.x;
    double onsy = onsmer.y;
    
    
    Bod intersekcia;
    
    double distodomna = vzdialenost(japos,intersekcia);
    double disodneho = vzdialenost(onpos,intersekcia);
    
    
    double kymonpride = 
    double kymjapridem =
    
    
    return (kymonpride==kymjapridem);
    
}*/

bool problemstena(Hrac &ja)
{
    FyzickyObjekt my = ja.obj;
    Bod nassmer = my.rychlost;
    
    //return false;
    bool p = false;
    
    if(my.pozicia.x < 200)
    {
        akcel.x = 200;
        p = true;
    }
    
    if(my.pozicia.y < 200)
    {
        akcel.y = 200;
        p = true;
    }
    
    if(my.pozicia.x > mapa.w - 200)
    {
        
        akcel.x = -200;
    
        p = true;
    }
    
    if(my.pozicia.y > mapa.h - 200)
    {
        akcel.y = -200;
        p = true;
    }
    
    if(my.pozicia.x < 400)
    {
        akcel.x = 150;
        p = true;
    }
    
    if(my.pozicia.y < 400)
    {
        akcel.y = 150;
        p = true;
    }
    
    if(my.pozicia.x > mapa.w - 400)
    {
        
        akcel.x = -150;
    
        p = true;
    }
    
    if(my.pozicia.y > mapa.h - 400)
    {
        akcel.y = -150;
        p = true;
    }
    
    return p;
    //if(my.pozicia.x<100)
    
}

void vystrel(Hrac ja, Bod kam, Bod rychlost=Bod())
{   kam.dist();
    float t=vzdialenost(kam,ja.obj.pozicia)/(vzdialenost((ja.obj.rychlost-rychlost),Bod())+STRELA_RYCHLOST);
    strela = kam-ja.obj.pozicia-(ja.obj.rychlost+rychlost)*t;
    
}

void utekaj(Bod odkial)
{
    Bod my = ja.obj.pozicia;
    Bod knemu;
    knemu.x = odkial.x-my.x;
    knemu.y = odkial.y - my.y;
    
    if(knemu.x > 0) akcel.x = -100;
    else if (knemu.x < 0) akcel.x = 100;
    
    if(knemu.y > 0) akcel.y = -100;
    else if (knemu.y < 0) akcel.y = 100;
}

void utekajodnajblizsieho()
{
    
    
    double posx = stav.hraci[0].obj.pozicia.x;
    double posy = stav.hraci[0].obj.pozicia.y;
    double rx = stav.hraci[0].obj.rychlost.x;
    double ry = stav.hraci[0].obj.rychlost.y;
    
    if(rx<10 || ry<10) return;
    
    while(posx>0 && posy>0 && posx<mapa.w && posy<mapa.h)
    {
        posx += rx;
        posy += ry;
    }
    
    
    
    Bod nas;
    nas.x=posx;
    nas.y=posy;
    if(vzdialenost(nas,ja.obj.pozicia) > 3*vzdialenost(Bod(),ja.obj.rychlost) ) return;
    fprintf(stderr, " %lf %lf \n", nas.x, nas.y);
    utekaj(nas);
}

long long citac=0,nasobx=1,nasoby=1;

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz

void zistiTah() {
    
    citac++;
    ja = stav.hraci[0];
    double celkova_rychlost = sqrt(ja.obj.rychlost.x*ja.obj.rychlost.x + ja.obj.rychlost.y*ja.obj.rychlost.y);
    
    if(celkova_rychlost == 300)
    {
        akcel.x = 0;
        akcel.y = 0;
    }
    else if (celkova_rychlost > 300)
    {
        if(ja.obj.rychlost.x > 150) akcel.x = -30;
        if(ja.obj.rychlost.x < -150) akcel.x = 30;
        if(ja.obj.rychlost.y > 150) akcel.y = -30;
        if(ja.obj.rychlost.y < -150) akcel.y = 30;
    }
    
   if (!problemstena(ja))
   {
       
       if(citac%3000==0) nasobx*=-1;
       if(citac%4500==0) nasoby *=-1;
       
        if(citac%50==0)
        {
           
            if(celkova_rychlost<300)
            {
                akcel.x+=rand()%70*nasobx;
                akcel.y+=rand()%70*nasoby;
            }
        }
   }
    
    /*FyzickyObjekt jaobj = ja.obj;
    Bod japos = ja.obj.pozicia;
    Bod jaacc = ja.obj.rychlost;
    strela.x = 0;
    strela.y = 0;
    vector < FyzickyObjekt > asteroidy;
    
    for(int i=0;i<stav.obj[0].size();++i) asteroidy.push_back(stav.obj[0][i]);
    
    for(int i=0;i<asteroidy.size();++i)
    {
        FyzickyObjekt kamen = asteroidy[i];
        Bod kdeje = kamen.pozicia;
        Bod kamide = kamen.rychlost;
        
        if(zrazima(jaobj,kamen))
        {
            strela.x = 3;
            strela.y = 3;
        }
        
    }*/
    
    //utekajodnajblizsieho();
    
    vector < FyzickyObjekt > asteroidy;
    for(int i=0;i<stav.obj[0].size();++i) asteroidy.push_back(stav.obj[0][i]);
    if(asteroidy.size()>0)vystrel(ja,asteroidy[0].pozicia, asteroidy[0].rychlost);
    //strela = /*asteroidy[0].pozicia(Bod()-ja.obj.pozicia)-ja.obj.rychlost;//+asteroidy[0].rychlost;
    else strela = Bod();
    int _min = 0; double _min_dist =100000;
    for(int i =1; i<stav.hraci.size(); i++){
        if((stav.hraci[i].obj.pozicia-ja.obj.pozicia).dist()<_min_dist){_min=i;}
        _min_dist=(stav.hraci[_min].obj.pozicia-ja.obj.pozicia).dist();
    }
    if(_min_dist<400){
        strela = stav.hraci[_min].obj.pozicia-ja.obj.pozicia;
    }
    
    prikaz.acc = akcel;   // akceleracia
    prikaz.ciel = strela;  // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
}





//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////





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
