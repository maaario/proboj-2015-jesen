#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <fstream>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"
#include "util.h"

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;
double mojeX;
double mojeY;
Bod mojaRychlost;
Bod lastRychlost;
Bod deltaRychlost = Bod();
//ofstream subor;

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
void Aktualizuj()
{
    mojeX = stav.hraci[0].obj.pozicia.x;
    mojeY = stav.hraci[0].obj.pozicia.y;
    mojaRychlost = stav.hraci[0].obj.rychlost;
    deltaRychlost = mojaRychlost - deltaRychlost; //ak + tak zrychlujem; ak - tak spomalujem
}

/*void ZmapujNepriatelov()
{
    
}*/

bool jeVKruhu(double cX, double cY, double r)
{
    double str3 =  sqrt((cX*cX) + (cY*cY));
    if (str3 < r)
        return true;
    else 
        return false;
}

Bod chodNa(double toX, double toY)
{
    double ostavaX = mojeX - toX;
    double ostavaY = mojeY - toY;
    Bod deltaR = Bod(50*cos(toX), 50*sin(toY));


    if (!jeVKruhu(toX, toY, 10))
    {
        deltaR = mojaRychlost;
    }
    else
    {
        deltaR = Bod(50*cos(toX), 50*sin(toY));
    }

    log("deltaR: %lf", deltaR.x);
    prikaz.acc = deltaR;   
}

void zistiTah() {
    /*
    - zbierat bonusy
    - zabijat nepriatelov a asteroidy
    - prezit
    - vyhybat sa strelam a narazom
    */
    //Ziskam si udaje o nepriateloch
    //vector<FyzikalnyObjekt*> objekty;
    //zoznamObjekty(stav,objekty);
    Aktualizuj();
    //subor << "MojeX: " << mojeX << endl;
    prikaz.acc = Bod(100, 100);
    //chodNa(100, 100);

}

int main() 
{
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

    unsigned seed = time(NULL) * getpid();
    srand(seed);

    nacitaj(cin,mapa);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    //subor.open ("debsubot.txt");

    while (cin.good()) {
        nacitaj(cin,stav);

        zistiTah();

        uloz(cout,prikaz);
        cout << endl;
    }

    //subor.close();
}
