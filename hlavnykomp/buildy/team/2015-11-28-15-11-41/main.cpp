#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <cmath>
#include <vector>
#include <stack>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som ja cislo 0
Prikaz prikaz;
FyzickyObjekt ja;

double d_t = 0.1;
stack<Bod> pozicie;

long long cas = 0;

Bod smer = Bod(1, 1);


double deg_to_rad(double deg)
{
    double res = deg/(180/3.242529);
    return res;
}

Bod get_random_point()
{
    long x_max = mapa.h;
    long y_max = mapa.w;
    long x = rand() % x_max;
    long y = rand() % y_max;
    x = rand() % 2 == 0 ? x : -x;
    y = rand() % 2 == 0 ? y : -y;
    return Bod(x, y);
}

Bod zmen_poziciu(Bod pozicia) {
    FyzickyObjekt ja = stav.hraci[0].obj;
    
    Bod akceleracia = Bod(2 * (pozicia.x - (ja.pozicia.x + ja.rychlost.y * d_t)) / (d_t * d_t),
                          2 * (pozicia.y - (ja.pozicia.y + ja.rychlost.y * d_t)) / (d_t * d_t)
                          );
    double predelit = abs(max(akceleracia.x, akceleracia.y) / 100.0);
    
    akceleracia.x /= predelit;
    akceleracia.y /= predelit; 
    
    return akceleracia;
}

int najblizsi_hrac(){
    double maximalna_vzdialenost = mapa.w * 2;
    maximalna_vzdialenost *= maximalna_vzdialenost;
    double best[2] = {10000000, 0};
    
    for (int i = 1; i < stav.hraci.size(); i++) {
        FyzickyObjekt protija = stav.hraci[i].obj;
        
        if (!protija.zije())
            continue;
        
        double vzdielnost = (ja.pozicia.x - protija.pozicia.x) * (ja.pozicia.x - protija.pozicia.x) +
                            (ja.pozicia.y - protija.pozicia.y) * (ja.pozicia.y - protija.pozicia.y);
        if (vzdielnost <= maximalna_vzdialenost)
            if (vzdielnost < best[0]) {
                best[0] = vzdielnost;
                best[1] = i;
            }
    }
    
    return best[1];
}

void hyb_sa_po_bodoch() {
    double vzdialenost = (stav.hraci[0].obj.pozicia.x - pozicie.top().x) * (stav.hraci[0].obj.pozicia.x - pozicie.top().x) + 
                         (stav.hraci[0].obj.pozicia.y - pozicie.top().y) * (stav.hraci[0].obj.pozicia.y - pozicie.top().y);
    double odchylka = 30;
    odchylka *= odchylka;
    if (vzdialenost < odchylka) {
        pozicie.pop();
    }
    
    prikaz.acc = zmen_poziciu(pozicie.top());
}

void kontroluj_okraje(){
    double minimalna_vzdialenost1 = 50;
    double zvacsenie_rychlosti1 = 200;
    double minimalna_vzdialenost2 = 125;
    double zvacsenie_rychlosti2 = 100;
    
    if (ja.pozicia.x < minimalna_vzdialenost1)
        prikaz.acc.x += zvacsenie_rychlosti1;
    if (ja.pozicia.x > mapa.w - minimalna_vzdialenost1)
        prikaz.acc.x -= zvacsenie_rychlosti1;
    if (ja.pozicia.x < minimalna_vzdialenost2)
        prikaz.acc.x += zvacsenie_rychlosti2;
    if (ja.pozicia.x > mapa.w - minimalna_vzdialenost2)
        prikaz.acc.x -= zvacsenie_rychlosti2;
    
    if (ja.pozicia.y < minimalna_vzdialenost1)
        prikaz.acc.y += zvacsenie_rychlosti1;
    if (ja.pozicia.y > mapa.h - minimalna_vzdialenost1)
        prikaz.acc.y -= zvacsenie_rychlosti1;
    if (ja.pozicia.y < minimalna_vzdialenost2)
        prikaz.acc.y += zvacsenie_rychlosti2;
    if (ja.pozicia.y > mapa.h - minimalna_vzdialenost2)
        prikaz.acc.y -= zvacsenie_rychlosti2;
}

void init() {
    pozicie.push(Bod(2000, 2000)); pozicie.push(Bod(2000, 0)); pozicie.push(Bod(300, 600)); pozicie.push(Bod(200, 200));
}

void zistiTah() {
    ja = stav.hraci[0].obj;
    cas += 1;

//    hyb_sa_po_bodoch();
    
//     if (ja.rychlost.x > 20 and ja.akceleracia.x > 0) {
//         prikaz.acc.x = 0;
//     }
//     else if (ja.rychlost.x < 20 and ja.akceleracia.x < 0) {
//         prikaz.acc.x = 0;
//     }
    
    // nahodny pohyb
    if (cas % 500)
        prikaz.acc = Bod(rand() % 200 * smer.x, rand() % 200 * smer.y);
    if (cas % 5000) {
        smer.x = rand() % 2 == 0 ? smer.x : -smer.x;
        smer.y = rand() % 2 == 0 ? smer.y : -smer.y;
    }
    
    kontroluj_okraje();
    
    
    // strelba bitches
    int zamiereny_hrac = najblizsi_hrac();
    if (zamiereny_hrac) {
        double rychlost_strely_v = 500;
        Bod ciel = stav.hraci[zamiereny_hrac].obj.pozicia - ja.pozicia;
        double uhol = atan2(ciel.y, ciel.x);
        Bod rychlost_strely = Bod(rychlost_strely_v * cos(uhol),
                                  rychlost_strely_v * sin(uhol));
        rychlost_strely = rychlost_strely - ja.rychlost;
        prikaz.ciel = rychlost_strely;
    }
    else
        prikaz.ciel = Bod(0, 0);
}

int main() {
    // v tejto funkcii su vseobecne veci, nemusite ju menit (ale mozte).

    unsigned seed = time(NULL) * getpid();
    srand(seed);

    nacitaj(cin,mapa);
    //fprintf(stderr, "START pid=%d, seed=%u\n", getpid(), seed);
    init();
    
    while (cin.good()) {
        nacitaj(cin,stav);
        
        zistiTah();

        uloz(cout, prikaz);
        cout << endl;
    }
}
