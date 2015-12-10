#include <iostream>
#include <unistd.h>
#include <ctime>
#include <random>
#include <cmath>
#include <vector>

using namespace std;

#include "common.h"
#include "marshal.h"
#include "update.h"

Mapa mapa;
Stav stav; //vzdy som ja cislo 0
Prikaz prikaz;
FyzickyObjekt ja;

double d_t = 0.1;

long long cas = 0;
double maximalna_rychlost = 200;
double maximalna_vzdialenost_strelby = 750 * 750;

Bod smer = Bod(1, 1);


double deg_to_rad(double deg)
{
    double res = deg/(180/3.141592);
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

int najblizsi_hrac(){
    double maximalna_vzdialenost = mapa.w * 2;
    maximalna_vzdialenost *= maximalna_vzdialenost;
    double best[2] = {10000000, 0};
    
    for (int i = 1; i < stav.hraci.size(); i++) {
        FyzickyObjekt protihrac = stav.hraci[i].obj;
        
        if (!protihrac.zije())
            continue;
        
        double vzdielnost = (ja.pozicia - protihrac.pozicia).dist2();
        if (vzdielnost <= maximalna_vzdialenost)
            if (vzdielnost < best[0]) {
                best[0] = vzdielnost;
                best[1] = i;
            }
        if (protihrac.rychlost == Bod(0, 0))
            return i;
    }
    
    return best[1];
}

int najblizssi_bod_v_poli(vector<FyzickyObjekt> pole) {
    double best[2] = {10000000, 0};
    
    for (int i = 1; i < pole.size(); i++) {
        Bod bod = pole[i].pozicia;
        
        double vzdielnost = (ja.pozicia - bod).dist2();
        if (vzdielnost <= maximalna_vzdialenost_strelby)
            if (vzdielnost < best[0]) {
                best[0] = vzdielnost;
                best[1] = i;
            }
    }
    
    return best[1];
}

Bod zmen_poziciu(Bod pozicia) {
    /*
    Bod akceleracia = Bod(2 * (pozicia.x - (ja.pozicia.x + ja.rychlost.x * d_t)) / (d_t * d_t),
                            2 * (pozicia.y - (ja.pozicia.y + ja.rychlost.y * d_t)) / (d_t * d_t)
                          );
    double predelit = abs(max(akceleracia.x, akceleracia.y) / 100.0);
    
    akceleracia.x /= predelit;
    akceleracia.y /= predelit; 
    */
    
    Bod rychlost = Bod((pozicia.x - ja.pozicia.x),
                       (pozicia.y - ja.pozicia.y)) * 0.5;
    double predelit = abs(max(rychlost.x, rychlost.y) / maximalna_rychlost);
    rychlost.x /= predelit;
    rychlost.y /= predelit;
    Bod akceleracia = (Bod(rychlost.x, rychlost.y) - ja.rychlost) * 5.0;
    
    return akceleracia;
}

void nahodny_pohyb() {
    if (cas % 500)
        prikaz.acc = Bod(rand() % int(maximalna_rychlost) * smer.x, rand() % int(maximalna_rychlost) * smer.y);
    if (cas % 5000) {
        smer.x = rand() % 2 == 0 ? smer.x : -smer.x;
        smer.y = rand() % 2 == 0 ? smer.y : -smer.y;
    }
}

void pohyb_po_bodoch() {
    int najblizsi_bod = najblizssi_bod_v_poli(stav.obj[ZLATO]);
    Bod bod = stav.obj[ZLATO][najblizsi_bod].pozicia;
    prikaz.acc = zmen_poziciu(bod);
}

void pohyb() {
    if (stav.obj[ZLATO].size() > 0)
        pohyb_po_bodoch();
    else
        nahodny_pohyb();
}

void kontroluj_okraje(){
    double minimalna_vzdialenost1 = 75;
    double zvacsenie_rychlosti1 = 200;
    double minimalna_vzdialenost2 = 150;
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

void strelba(FyzickyObjekt zamiereny_hrac) {
    double rychlost_strely_v = 500;
    Bod ciel = zamiereny_hrac.pozicia - ja.pozicia;
    double uhol = atan2(ciel.y, ciel.x);
    Bod rychlost_strely = Bod(rychlost_strely_v * cos(uhol),
                              rychlost_strely_v * sin(uhol));
    rychlost_strely = rychlost_strely - ja.rychlost;
    prikaz.ciel = rychlost_strely;
}

void zistiTah() {
    ja = stav.hraci[0].obj;
    cas += 1;

    pohyb();
    kontroluj_okraje();
    
    if ((ja.rychlost.x > maximalna_rychlost and prikaz.acc.x > 0) or
        (ja.rychlost.x < -maximalna_rychlost and prikaz.acc.x < 0) or
        (ja.rychlost.y > maximalna_rychlost and prikaz.acc.y > 0) or
        (ja.rychlost.y < -maximalna_rychlost and prikaz.acc.y < 0)) {
            Bod rychlost = ja.rychlost;
            double predelit = abs(max(rychlost.x, rychlost.y) / maximalna_rychlost);
            rychlost.x /= predelit;
            rychlost.y /= predelit;
            prikaz.acc = Bod(-rychlost.x / d_t, -rychlost.y / d_t);
    }
    
    // strelba bitches
    int zamiereny_hrac_i = najblizsi_hrac();
    if (zamiereny_hrac_i) {
        FyzickyObjekt zamiereny_hrac = stav.hraci[zamiereny_hrac_i].obj;
        double vzdialenost = (ja.pozicia - zamiereny_hrac.pozicia).dist2();
        
        if (vzdialenost <= maximalna_vzdialenost_strelby)
            if (vzdialenost > 300 * 300 and stav.hraci[0].zasobnik >= 3)
                    return;
            else
                strelba(zamiereny_hrac);
        /*if ((ja.pozicia - stav.hraci[zamiereny_hrac].obj.pozicia).dist2() <= maximalna_vzdialenost_strelby) {
            if ((ja.pozicia - stav.hraci[zamiereny_hrac].obj.pozicia).dist2() > 300 * 300)
                if (stav.hraci[0].zasobnik >= 3)
                    return;
                
            double rychlost_strely_v = 500;
            Bod ciel = stav.hraci[zamiereny_hrac].obj.pozicia - ja.pozicia;
            double uhol = atan2(ciel.y, ciel.x);
            Bod rychlost_strely = Bod(rychlost_strely_v * cos(uhol),
                                      rychlost_strely_v * sin(uhol));
            rychlost_strely = rychlost_strely - ja.rychlost;
            prikaz.ciel = rychlost_strely;
        }*/
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
    
    while (cin.good()) {
        nacitaj(cin,stav);
        
        zistiTah();

        uloz(cout, prikaz);
        cout << endl;
    }
}
