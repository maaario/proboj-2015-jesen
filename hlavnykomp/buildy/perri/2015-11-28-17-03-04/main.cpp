#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <math.h>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

#define PI 3.14

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;
double mojeX;
double mojeY;
double mojeLastAccX = 100;
double mojeLastAccY = 100;
double mojeAccX = 0;
double mojeAccY = 100;
Bod mojaRychlost;
Bod lastRychlost;
Bod deltaRychlost = Bod();
double XOdSteny = 100;
double YOdSteny = 100;
bool handleSt, handleSl = false;
Bod chcemIst [1000];
int chIP = 0;
Bod blizko = Bod(8888888, 666666666666);

//ofstream subor;

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void Aktualizuj()
{
    mojeX = stav.hraci[0].obj.pozicia.x;
    mojeY = stav.hraci[0].obj.pozicia.y;
    mojaRychlost = stav.hraci[0].obj.rychlost;
    deltaRychlost = mojaRychlost - deltaRychlost; //ak + tak zrychlujem; ak - tak spomalujem
    XOdSteny = 2 * mojaRychlost.x;
    YOdSteny = 2 * mojaRychlost.y;
    //cerr << "mojeX: " << mojeX << " mojeY: " << mojeY << " mojaRychlost(x,y):" << mojaRychlost.x << " " << mojaRychlost.y << endl << "mojaAcc: " << mojeAccX << mojeAccY << endl;
}

void nacitajObjekty()
{
    for(int i = 0; i < mapa.objekty.size(); i++)
    {
        if (mapa.objekty[i].typ == ZLATO)
        {
            chcemIst[chIP] = mapa.objekty[i].pozicia;
            chIP++;
            cerr << "ZLATO I: "<<i<<" chcemIst X: "<<chcemIst[chIP].x<<" y: "<<chcemIst[chIP].y<<endl;
        }
        else if (mapa.objekty[i].typ == PLANETA)
        {
            
        }
    }
}

Bod zistiKam()
{
    if (chIP != 0)
    { 
        for (int i = 0; i <= chIP; i++)
        {
            double dx = mojeX - chcemIst[i].x;
            double dy = mojeY - chcemIst[i].y;

            cerr << "bliz pred x: "<<blizko.x<<" y: "<<blizko.y<<" dx: "<<dx<<" dy: "<<dy<<endl;
            if ((abs(mojeX - blizko.x)>abs(dx))&&(abs(mojeY - blizko.y)>abs(dy)))
            {
                blizko = Bod(abs(dx), abs(dy));
            }
            cerr << "bliz po x: "<<blizko.x<<" y: "<<blizko.y<<endl;
        }
    }
}

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
void zistiTah() {
  Aktualizuj();
  chIP = 0;
  nacitajObjekty();
  zistiKam();
  prikaz.acc = kontroler(Bod(mojeX, mojeY), Bod(100, 100), zistiKam());
  prikaz.ciel = Bod();  // ciel strelby (relativne na poziciu lode), ak je (0,0) znamena to nestrielat
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
