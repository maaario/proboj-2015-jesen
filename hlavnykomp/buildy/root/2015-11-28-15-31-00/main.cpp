#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"
#include "cmath"

int pi = 3.1415926535;
int enemyId = 0;
int interval = 0;
int por = rand() % 360;
Bod smer;

Mapa mapa;
Stav stav; //vzdy som hrac cislo 0
Prikaz prikaz;

Bod kontroler(Bod pozicia, Bod rychlost, Bod cielovaPozicia) {
    Bod rozdielPozicie = cielovaPozicia - pozicia;
    Bod cielovaRychlost = rozdielPozicie * 1.0;

    // umele obmedzenie maximalnej rychlosti pre lepsiu kontrolu
    double pomerKMojejMaximalnejRychlosti = 150.0 / cielovaRychlost.dist();
    if (pomerKMojejMaximalnejRychlosti > 1.0) {
        cielovaRychlost = cielovaRychlost * pomerKMojejMaximalnejRychlosti;
    }

    Bod rozdielRychlosti = cielovaRychlost - rychlost;
    Bod cielovaAkceleracia = rozdielRychlosti * 5.0;
    return cielovaAkceleracia;
}

int getEnemyId() {
    double length = -1;
    int id;
    for (unsigned int i = 1; i <= stav.hraci.size(); i++) {
        if (stav.hraci[i].obj.zije()) {
            if (length == -1) {
                length = (stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist();
            } else if (length > (stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist()) {
                length = (stav.hraci[i].obj.pozicia - stav.hraci[0].obj.pozicia).dist();
            }
            id = stav.hraci[i].obj.id;
        }
    }
    return id;
}
/*
int getShotId() {
    double length = -1;
    int id;
    for (unsigned int i = 1; i <= stav.obj[3].size(); i++) {

        if (length == -1) {
            length = (stav.obj[3][i].pozicia- stav.hraci[0].obj.pozicia).dist();
        } else if (length > (stav.obj[3][i].pozicia - stav.hraci[0].obj.pozicia).dist()) {
            length = (stav.obj[3][i].pozicia - stav.hraci[0].obj.pozicia).dist();
        }
        id = stav.obj[3][i].id;

    }
    return id;
}*/

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;
/*
void getDirection(int alfa){
    por = alfa;
    double uhol = por;
    if (por % 180 == 0 && por / 180 == 1) {
        smer = Bod(0, -100);
    }
    else if (por % 180 == 0 && por / 360 == 1) {
        smer = Bod(0, 100);
    }
    else if (por > 180) {
        smer = Bod(-100, -100 * cos(uhol * pi / 180) / sin(uhol * pi / 180));
    }
    else {
        smer = Bod(100, 100* cos(uhol * pi / 180)/sin(uhol * pi / 180));
        cerr << "sin " << cos(uhol * pi / 180) << endl;
    }
}*/

void direction(){
    int angul;
    double uhol = por;
    if(interval != 50){
        angul = por;
        interval++;
    }
    else {
        por = rand() % 360;
        interval = 0;
    }
    if (por % 180 == 0 && por / 180 == 1) {
        smer = Bod(0, -50);
    }
    else if (por % 180 == 0 && por / 360 == 1) {
        smer = Bod(0, 50);
    }
    else if (por > 180) {
        smer = Bod(-50, -50 * cos(uhol * pi / 180) / sin(uhol * pi / 180));
    }
    else {
        smer = Bod(50, 50* cos(uhol * pi / 180)/sin(uhol * pi / 180));
    }
}

void zistiTah(){
    double uhol = por;
    
    if((stav.hraci[0].obj.pozicia.x + 200) > (mapa.w)){
        smer = Bod(-100, 0);
    }
    if((stav.hraci[0].obj.pozicia.y + 200) > (mapa.h)){
        smer = Bod(0, -100);
    }
    if((stav.hraci[0].obj.pozicia.y - 200) < 0){
        smer = Bod(0, 100);
    }
    if((stav.hraci[0].obj.pozicia.x - 200) < 0){
        smer = Bod(100, 0);
        cerr << "dosiel som";
    }
    else {
        direction();
    }
    prikaz.acc = smer;
        
    //cerr << "angul: " << uhol*pi/180 << " smer: " << smer.x << " " << smer.y << endl << cos(uhol*pi/ 180) / sin(uhol * pi / 180) << " sin: " << sin(uhol * pi / 180) << " cos: " << cos(uhol * pi / 180) << endl;
    

/*
    if (por % 180 == 0 && por / 180 == 1) {
        smer = Bod(0, -1);
    } else if (por % 180 == 0 && por / 360 == 1) {
        smer = Bod(0, 1);
    } else if (por > 180) {
        smer = Bod(-1, -1 * cos(uhol * pi / 180) / sin(uhol * pi / 180));
    } else {
        smer = Bod(1, cos(uhol * pi / 180)/sin(uhol * pi / 180));
    }*/
    if(enemyId == 0){
        enemyId = getEnemyId();
    }
    if(!stav.hraci[enemyId].obj.zije()){
        enemyId = getEnemyId();
    }
    smer = stav.hraci[enemyId].obj.pozicia - stav.hraci[0].obj.pozicia + stav.hraci[enemyId].obj.rychlost - stav.hraci[0].obj.rychlost;
    prikaz.ciel = smer;
}

int main(){
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

    unsigned seed = time(NULL) * getpid();
    srand(seed);

    nacitaj(cin, mapa);
    fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);

    while (cin.good()) {
        nacitaj(cin, stav);

        zistiTah();

        uloz(cout, prikaz);
        cout << endl;
    }
}
