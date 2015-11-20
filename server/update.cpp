
#include <ostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

#include "common.h"
#include "update.h"
#include "util.h"

static ostream* g_observation;
void zapniObservation(ostream* observation) { g_observation = observation; }

template<class T1, class T2>
bool zrazka (const T1& A, const T2& B) {
  Bod spojnica = A.pozicia - B.pozicia;
  return A.polomer + B.polomer > spojnica.dist();
}

Bod odpal1 (const FyzikalnyObjekt& A, const FyzikalnyObjekt& B) {
  // zrazka 1. typu, co oddeluje mergnute objekty
  //
  // vracia zrychlenie A v dosledku kolizie

  if (!zrazka(A,B)) {
    return Bod();
  }
  if (A.koliznyLevel > B.koliznyLevel) {
    return Bod();
  }
  double cast= (A.koliznyLevel==B.koliznyLevel ? 1.0 : 2.0);
  
  double dotykVzdial = A.polomer+B.polomer;
  Bod spojnica = A.pozicia - B.pozicia;
  Bod ciel = spojnica * (dotykVzdial / spojnica.dist());

  Bod projekcia = (A.pozicia+A.rychlost)-(B.pozicia+B.rychlost);
  projekcia = projekcia*spojnica;

  Bod vysl = (ciel-projekcia)*cast;
  if (vysl/spojnica < 0) {
    return Bod();
  }
  return vysl;
}

Bod odpal2 (const FyzikalnyObjekt& A, const FyzikalnyObjekt& B) {
  // zrazka 2. typu, co NEoddeluje mergnute objekty
  //
  // vracia zrychlenie A v dosledku kolizie

  if (!zrazka(A,B)) {
    return Bod();
  }
  if (A.koliznyLevel > B.koliznyLevel) {
    return Bod();
  }
  double cast= (A.koliznyLevel==B.koliznyLevel ? 1.0 : 2.0);

  Bod spojnica= A.pozicia-B.pozicia;
  Bod projekcia= (A.pozicia+A.rychlost)-(B.pozicia+B.rychlost);
  projekcia= projekcia*spojnica;

  Bod vysl= (spojnica-projekcia)*cast;
  if (vysl/spojnica < 0) {
    return Bod();
  }
  return vysl;
}



void odsimuluj(const Mapa& mapa, Stav& stav, vector<Prikaz>& akcie) {
  // axiomy:
  // kazdy objekt musi byt zivy aspon 1 jednotku casu
  // vznikajuce objekty ovplyvnuju a su ovplyvnovane az nasledujucim stavom
  // laser nie je vznikajuci objekt, ten ovplyvnuje aktualny stav

  // uprav prikazy klientov:
  // aby neakcelerovali viac ako LOD_MAX_ACC
  // aby pouzivali/strielali len to, coho naboje maju
  // 
  for (int i=0; i<(int)akcie.size(); i++) {
    double pomer=akcie[i].acc.dist()/LOD_MAX_ACC;
    if (pomer>1) {
      akcie[i].acc=akcie[i].acc*(1.0/pomer);
    }

    int pal= akcie[i].pal;
    if (pal<0 || pal>=DRUHOV_ZBRANI || stav.hraci[i].zbrane[pal]<=0 ||
      stav.hraci[i].cooldown>0 || stav.hraci[i].obj.pozicia==akcie[i].ciel)
    {
      akcie[i].pal=-1;
    }

    for (int j=0; j<DRUHOV_VECI; j++) {
      if (stav.hraci[i].veci[j]<=0) {
        akcie[i].pouzi[j]=0;
      }
    }
  }

  // urob zoznam vacsiny objektov
  //
  vector<FyzikalnyObjekt*> objekty;
  vector<Bod> zrychl;
  for (int t=0; t<NORM_TYPOV; t++) {
    int typ = norm_typy[t];
    for (FyzikalnyObjekt& obj : stav.obj[typ]) {
      objekty.push_back(&obj);
      zrychl.push_back(Bod());
    }
  }
  for (FyzikalnyObjekt& obj : stav.obj[BOSS]) {
    // so far, bossovia su indestructible
    // aby sa mohli hybat hlupo (burat do hviezd)
    //
    objekty.push_back(&obj);
    Bod kam(obj.pozicia);
    double best= INF;
    for (int i=0; i<(int)stav.hraci.size(); i++) {
      Bod kde= stav.hraci[i].obj.pozicia;
      double vzdial= (kde-obj.pozicia).dist();
      if (vzdial < best) {
        best = vzdial;
        kam=kde;
      }
    }
    kam= kam-obj.pozicia;
    kam= kam*(BOSS_MAX_ACC/best);
    zrychl.push_back(kam);
  }
  for (Vec& vec : stav.veci) {
    objekty.push_back(&vec.obj);
    zrychl.push_back(Bod());
  }
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    objekty.push_back(&stav.hraci[i].obj);
    zrychl.push_back(akcie[i].acc);
  }

  // zrazanie objektov
  //
  for (int i=0; i<(int)objekty.size(); i++) {
    FyzikalnyObjekt* prvy=objekty[i];
    vector<double> skore(akcie.size(),0.0);
    for (int j=0; j<(int)objekty.size(); j++) {
      if (i==j) {
        continue;
      }
      FyzikalnyObjekt* druhy=objekty[j];
      
      Bod acc = odpal1(*prvy, *druhy);
      Bod antiacc = odpal1(*druhy, *prvy);
      double silaZrazky = acc.dist()+antiacc.dist();
      
      if (! prvy->neznicitelny() ) {
        double damage= druhy->sila*silaZrazky;
        prvy->zivoty -= damage;
        if (druhy->owner>=0 && druhy->owner<(int)skore.size()) {
          skore[druhy->owner]+= damage;
        }
      }
      zrychl[i]= zrychl[i]+acc;
    }

    // hori? (je vo vybuchu?)
    //
    if (! prvy->neznicitelny() ) {
      for (Vybuch& bum : stav.vybuchy) {
        if (!zrazka(bum,*prvy)) {
          continue;
        }
        double damage= bum.sila;
        prvy->zivoty-= damage;
        skore[bum.owner]+= damage;
      }
    }

    for (int j=0; j<(int)skore.size(); j++) {
      if (!prvy->zije()) {
        skore[j]+= body_za_znic[prvy->typ];
      }
      if (j == prvy->owner) {
        skore[j]*= -1;
      }
      stav.hraci[j].skore+= skore[j];
    }
  }
  for (int i=0; i<(int)objekty.size(); i++) {
    objekty[i]->rychlost = objekty[i]->rychlost + zrychl[i];
  }

  // vypal zo zbrani
  //
  vector<FyzikalnyObjekt*> vznikleObjekty;
  for (int i=0; i<(int)akcie.size(); i++) {
    int pal= akcie[i].pal;
    Bod ciel= akcie[i].ciel;
    
    if (pal==-1) {
      continue;
    }
    int owner= stav.hraci[i].obj.owner;
    Bod poz= stav.hraci[i].obj.pozicia;
    Bod rychl= stav.hraci[i].obj.rychlost;
    double polomer= stav.hraci[i].obj.polomer;
    
    Bod smer= ciel-poz;
    double safedist= polomer+1.0 + (pal<DRUHOV_PROJ ? z_polomer[pal] : 0);
    Bod spawn= poz + smer*(safedist/smer.dist());

    if (pal<DRUHOV_PROJ) {
      smer=smer*(z_rychlost[pal]/smer.dist());
      Bod p_rychl= rychl+smer;
      FyzikalnyObjekt strela( zbran_na_proj(pal), owner,spawn, p_rychl, z_polomer[pal], z_kolizny_lv[pal], z_sila[pal], z_zivoty[pal]);
      stav.obj[PROJEKTIL].push_back(strela);
      vznikleObjekty.push_back(&stav.obj[PROJEKTIL].back());

      stav.hraci[i].cooldown= COOLDOWN;
    }
    else {
      if (pal==ZBRAN_LASER) {
        // laser striela okamzite, prechadza cez vsetko
        // a je relativne slaby
        //
        for (FyzikalnyObjekt* ptr : objekty) {
          Bod spojnica= ptr->pozicia-spawn;
          Bod pata= spojnica*smer;
          Bod kolmica= spojnica-pata;
          if (pata/spojnica < 0) {
            continue;
          }
          if (kolmica.dist() >= ptr->polomer) {
            continue;
          }
          ptr->zivoty -= LASER_SILA;
        }
      }
      // sem sa daju nastrkat dalsie speci zbrane
      //
    }
  }
  // medzi objekty pridaj tie, co prave vznikli
  while (!vznikleObjekty.empty()) {
    objekty.push_back(vznikleObjekty.back());
    vznikleObjekty.pop_back();
  }


  // udrz objekty v hracom poli
  //
  int dx[4]={0,1,0,-1};
  int dy[4]={1,0,-1,0};
  for (FyzikalnyObjekt* ptr : objekty) {
    for (int smer=0; smer<4; smer++) {
      double x= ptr->pozicia.x;
      double y= ptr->pozicia.y;
      
      switch (dx[smer]) {
        case -1: x= -SENTINEL_POLOMER; break;
        case 1: x=mapa.w+SENTINEL_POLOMER;
      }
      switch (dy[smer]) {
        case -1: y= -SENTINEL_POLOMER; break;
        case 1: y=mapa.h+SENTINEL_POLOMER;
      }
      FyzikalnyObjekt sentinel(-1,-1,Bod(x,y),Bod(),SENTINEL_POLOMER,INF,SENTINEL_SILA,INF);
      //log("vzdialenost od sentinelu %d je %f",smer,(sentinel.pozicia - ptr->pozicia).dist());
      Bod acc= odpal2(*ptr,sentinel);
      ptr->rychlost= ptr->rychlost+acc;
      if (! ptr->neznicitelny() ) {
        ptr->zivoty -= sentinel.sila*acc.dist();
      }
    }
  }

  // ziskaj pickupy (veci)
  //
  for (Hrac& hrac : stav.hraci) {
    for (Vec& bonus : stav.veci) {
      if (!zrazka(hrac.obj, bonus.obj)) {
        continue;
      }
      int typ= bonus.typ;
      if (typ<DRUHOV_ZBRANI) {
        hrac.zbrane[typ]+=bonus.naboje;
      }
      else {
        typ-= DRUHOV_ZBRANI;
        hrac.veci[typ]+=bonus.naboje;
      }
    }
  }


  // pohni objektami
  //
  for (FyzikalnyObjekt* ptr : objekty) {
    ptr->pozicia= ptr->pozicia + ptr->rychlost;
  }

  // pochovaj mrtve
  // asteroidy, planety, hviezdy, bossov, projektily, veci
  //
  for (int t=0; t<STAV_TYPOV; t++) {
    for (int i=(int)stav.obj[t].size()-1; i>=0; i--) {
      if (stav.obj[t][i].zije()) {
        continue;
      }
      stav.obj[t][i]= stav.obj[t].back();
      stav.obj[t].pop_back();
    }
  }
  for (int i=(int)stav.veci.size()-1; i>=0; i--) {
    if (stav.veci[i].zije()) {
      continue;
    }
    stav.veci[i]= stav.veci.back();
    stav.veci.pop_back();
  }

  // cleanup
  for (Hrac& hrac : stav.hraci) {
    hrac.cooldown--;
  }

  //*
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      log("hrac %d (majitel lode je hrac %d) je mrtvy",stav.hraci[i].obj.id,stav.hraci[i].obj.owner);
      continue;
    }
    log("hrac %d (majitel lode je hrac %d) na pozicii %f,%f sa hybe rychlostou %f,%f a ma %f zivotov",
      stav.hraci[i].obj.id,stav.hraci[i].obj.owner,stav.hraci[i].obj.pozicia.x,
      stav.hraci[i].obj.pozicia.y,stav.hraci[i].obj.rychlost.x,stav.hraci[i].obj.rychlost.y,
      stav.hraci[i].obj.zivoty);
  }
  //*/

  stav.cas++;
}



bool pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov) {
  stav.cas = 0;

  if ((int)mapa.spawny.size() < pocKlientov) {
    log("prilis malo spawnov v mape");
    return false;
  }
  random_shuffle(mapa.spawny.begin(), mapa.spawny.end());
  for (int i=0; i<pocKlientov; i++) {
    Hrac novy(mapa.spawny[i]);
    stav.hraci.push_back( novy );
  }
  mapa.spawny.clear();

  for (int i=0; i<(int)mapa.objekty.size(); i++) {
    int typ = mapa.objekty[i].typ;
    if (typ > MAPA_TYPOV || typ < 0) {
      log("neznamy objekt vo vesmire");
      return false;
    }
    stav.obj[typ].push_back( mapa.objekty[i] );
  }
  mapa.objekty.clear();

  for (int i=0; i<(int)mapa.veci.size(); i++) {
    stav.veci.push_back( mapa.veci[i] );
  }
  mapa.veci.clear();

  return true;
}
