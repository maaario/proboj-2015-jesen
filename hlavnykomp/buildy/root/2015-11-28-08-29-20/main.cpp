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
int enemyId;
int interval;
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

int getEnemyId() {
    double length = -1;
    int id;
    for (int i = 1; i <= stav.hraci.size(); i++) {
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

int getShotId() {
    double length = -1;
    int id;
    for (int i = 1; i <= stav.obj[3].size(); i++) {

        if (length == -1) {
            length = (stav.obj[3][i].pozicia- stav.hraci[0].obj.pozicia).dist();
        } else if (length > (stav.obj[3][i].pozicia - stav.hraci[0].obj.pozicia).dist()) {
            length = (stav.obj[3][i].pozicia - stav.hraci[0].obj.pozicia).dist();
        }
        id = stav.obj[3][i].id;

    }
    return id;
}

// main() zavola tuto funkciu, ked chce vediet, aky prikaz chceme vykonat,
// co tato funkcia rozhodne pomocou toho, ako nastavi prikaz;

void zistiTah() {
    int angul = rand() % 360;

    Bod smer;

    //    cerr << getEnemyId() << endl;

    //    smer = Bod(1,cos(angul*pi/180)/sin(angul*pi/180));

    //    if((stav.hraci[getEnemyId()].obj.pozicia - stav.hraci[0].obj.pozicia).dist() < 50){
    //        prikaz.acc = stav.hraci[getEnemyId()].obj.pozicia + stav.hraci[0].obj.pozicia;
    //    }else{
    //        prikaz.acc = stav.hraci[getEnemyId()].obj.pozicia - stav.hraci[0].obj.pozicia;
    //    }

    smer = Bod(-100, -100 * cos(angul * pi / 180) / sin(angul * pi / 180));
//    cerr << getShotId() << endl;
    //    if (stav.obj['STRELA'].zije()) {
    //        smer = (stav.obj.pozicia).dist();
    //        cerr << smer << endl;

    //    }



    if (interval > 200 && interval < 600) {
        smer = Bod(100, 100 * cos(angul * pi / 180) / sin(angul * pi / 180));

    }
    if (interval == 600) {
        interval = 0;
    }

    interval++;

    prikaz.acc = smer;



    if (angul % 180 == 0 && angul / 180 == 1) {
        smer = Bod(-1, 0);
    } else if (angul % 180 == 0 && angul / 360 == 1) {
        smer = Bod(1, 0);
    } else if (angul > 180) {
        smer = Bod(-1, -1 * cos(angul * pi / 180) / sin(angul * pi / 180));
    } else {
        smer = Bod(1, cos(angul * pi / 180) / sin(angul * pi / 180));
    }
    prikaz.ciel = smer;
}

int main() {
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
