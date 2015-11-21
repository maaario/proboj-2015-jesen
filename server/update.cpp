
#include <ostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <map>
using namespace std;

#include "common.h"
#include "update.h"
#include "util.h"

#define PRESNOST 100

static int volne_zap_id = -INF;

#define NORM_TYPOV 4
const static int kNormTypy[NORM_TYPOV] =
  {ASTEROID,PLANETA,HVIEZDA,PROJ_BEGIN};

#define SENTINEL_POLOMER 987654.0

#define OBSERVE_VYBUCH 13
#define OBSERVE_LASER 14
#define NOTE_UMIERA -1
#define NOTE_STIT -2

static ostream* g_observation;
static int frame_t;
void zapniObservation(ostream* observation, int ft) {
  g_observation = observation;
  frame_t=ft;
}


double rand_float (double d) {
  return (double)(rand()%int(1.0 + d*PRESNOST))/(double)PRESNOST;
}


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



Vec vytvorVec(Bod poz,Bod rychl) {
  int typ= rand()%(DRUHOV_ZBRANI+DRUHOV_VECI);
  int nabojov= int(kV_nabojov[typ]/2);
  if (nabojov<1) {
    nabojov=1;
  }
  nabojov+= rand()%(1+kV_nabojov[typ]-nabojov);
  Vec res(poz,VEC_BEGIN+typ,nabojov);
  res.obj.rychlost= rychl;
  return res;
}

FyzikalnyObjekt vytvorAst(Bod poz,Bod rychl, double r) {
  double zivoty= AST_ZIV_RATE*r;
  FyzikalnyObjekt res(ASTEROID,-1, poz,rychl,r, AST_KOLIZNY_LV, AST_SILA, zivoty);
  return res;
}

void rozpad(const FyzikalnyObjekt& obj, vector<FyzikalnyObjekt>& vznikleObjekty, vector<Vec>& vznikleVeci) {
  if (obj.typ!=ASTEROID && obj.typ!=PLANETA) {
    return;
  }
  
  if (obj.polomer/2 < AST_MIN_R) {
    if (rand_float(1.0) < obj.obsah()*AST_DROP_RATE) {
      vznikleVeci.push_back(vytvorVec(obj.pozicia,obj.rychlost));
    }
    return;
  }
  vznikleObjekty.push_back(vytvorAst(obj.pozicia,obj.rychlost,obj.polomer));
  
  vector<pair<bool,Bod> > pozy;
  {
    vector<int> moznePozy;
    for (int i=0; i<PRESNOST; i++) {
      moznePozy.push_back(i);
    }
    while (moznePozy.size()>0) {
      int n= moznePozy.size();
      int kde=rand()%n;
      kde= moznePozy[kde];
      double uhol= 2.0*PII*(double)kde/(double)PRESNOST;
      Bod pozicia(0.75*obj.polomer*sin(uhol), 0.75*obj.polomer*cos(uhol));

      bool jeVec= (rand_float(1.0) < obj.obsah()*AST_DROP_RATE);
      pozy.push_back(make_pair(jeVec,pozicia));
      
      int obsadenyUsek= PRESNOST;
      if (jeVec) {
        obsadenyUsek*= VEC_POLOMER;
        obsadenyUsek/=(0.25*obj.polomer);
      }
      obsadenyUsek/=7;

      vector<int> novePozy;
      for (int& val : moznePozy) {
        if (abs(val-kde)<obsadenyUsek) {
          continue;
        }
        novePozy.push_back(val);
      }
      moznePozy.swap(novePozy);
    }
  }
  for (int i=0; i<(int)pozy.size(); i++) {
    Bod poz= pozy[i].second;
    double acc= rand_float(AST_ROZPAD_ACC);
    double uhol= rand_float(2.0*PII);
    Bod rychl(acc*sin(uhol),acc*cos(uhol));

    bool jeVec= pozy[i].first;
    if (jeVec) {
      vznikleVeci.push_back(vytvorVec(poz,rychl));
    }
    else {
      if (obj.polomer/4 >= AST_MIN_R) {
        vznikleObjekty.push_back(vytvorAst(poz,rychl,obj.polomer/4));
      }
    }
  }
}


struct obraz {
  int typ;
  int owner;
  Bod zac,kon;
  double polomer;
  int note;

  obraz () {}
  obraz (int t,int own, Bod poz, double r, int nt) :
    typ(t), owner(own), zac(poz), kon(poz), polomer(r), note(nt) {}
  obraz (const FyzikalnyObjekt& obj) {
    typ=obj.typ;
    owner=obj.owner;
    zac=obj.pozicia;
    kon=obj.pozicia;
    polomer=obj.polomer;
    note=0;
    if (!obj.zije()) {
      note=NOTE_UMIERA;
    }
    if (obj.stit>0) {
      note=NOTE_STIT;
    }
  }
  obraz (const Vybuch& bum) {
    typ= OBSERVE_VYBUCH;
    owner=-1;
    zac=bum.pozicia;
    kon=bum.pozicia;
    polomer=bum.polomer;
    note=bum.faza;
  }
  obraz (int own,Bod start,Bod tgt) {
    typ= OBSERVE_LASER;
    owner= own;
    zac= start;
    kon= tgt;
    polomer= 0;
    note= 0;
  }
};

int konvertDoStavu (int typ) {
  switch (typ) {
    case ASTEROID: return ASTEROID;
    case PLANETA: return PLANETA;
    case HVIEZDA: return HVIEZDA;
    case BOSS: return BOSS;
    case BROK:
    case BOMBA: return PROJ_BEGIN;
  }
  return -INF;
}

map<int,obraz> vidim;

void zazrel(FyzikalnyObjekt& obj) {
  if (vidim.count(obj.id)) {
    vidim[obj.id].kon= obj.pozicia;
    if (!obj.zije()) {
      vidim[obj.id].note= NOTE_UMIERA;
    }
    if (obj.stit > 0) {
      vidim[obj.id].note= NOTE_STIT;
    }
  }
  else {
    vidim[obj.id]= obraz(obj);
  }
}

void zazrel(Vybuch& bum) {
  if (vidim.count(bum.id)) {
    vidim[bum.id].kon= bum.pozicia;
  }
  else {
    vidim[bum.id]= obraz(bum);
  }
}

void zaznamuj(Stav& stav) {
  for (int t=0; t<STAV_TYPOV; t++) {
    for (FyzikalnyObjekt& obj : stav.obj[t]) {
      zazrel(obj);
    }
  }
  for (Vec& item : stav.veci) {
    zazrel(item.obj);
  }
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    zazrel(hrac.obj);
  }
  for (Vybuch& bum : stav.vybuchy) {
    zazrel(bum);
  }
}

void vypis() {
  if (vidim.empty()) {
    return;
  }
  for (pair<const int,obraz>& parik : vidim) {
    obraz* ptr= &parik.second;
    *g_observation << ptr->typ << " " << ptr->owner << " " << ptr->zac.x
      << " " << ptr->zac.y << " " << ptr->kon.x << " " << ptr->kon.y
      << " " << ptr->polomer << " " << ptr->note << "\n";
  }
  *g_observation << endl;
  vidim.clear();
}

void opravPrikazy(Stav& stav,vector<Prikaz>& akcie) {
  // uprav prikazy klientov:
  // aby neakcelerovali viac ako LOD_MAX_ACC (nasledne hned upravim rychlost hracov)
  // aby pouzivali/strielali len to, coho naboje maju
  //
  log("opavujem prikazy klientov");
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    double pomer=akcie[i].acc.dist()/LOD_MAX_ACC;
    if (pomer>1) {
      akcie[i].acc=akcie[i].acc*(1.0/pomer);
    }
    stav.hraci[i].obj.rychlost= stav.hraci[i].obj.rychlost+akcie[i].acc;

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
}

void udrz(FyzikalnyObjekt& obj, const Mapa& mapa) {
  // udrz objekty v hracom poli
  // aby platilo: kazdy objekt mimo mapy sa nevie hybat dalej od mapy
  //
  int dx[4]={0,1,0,-1};
  int dy[4]={1,0,-1,0};
  for (int smer=0; smer<4; smer++) {
    double x= obj.pozicia.x;
    double y= obj.pozicia.y;
    switch (dx[smer]) {
      case -1: x= -SENTINEL_POLOMER; break;
      case 1: x=mapa.w+SENTINEL_POLOMER;
    }
    switch (dy[smer]) {
      case -1: y= -SENTINEL_POLOMER; break;
      case 1: y=mapa.h+SENTINEL_POLOMER;
    }
    FyzikalnyObjekt sentinel(-1,-1,Bod(x,y),Bod(),SENTINEL_POLOMER,INF,SENTINEL_SILA,INF);
    Bod acc= odpal2(obj,sentinel);
    obj.rychlost= obj.rychlost+acc;
    if (! obj.neznicitelny() ) {
      obj.zivoty -= sentinel.sila*acc.dist();
    }
  }
}

void pohniBossom(FyzikalnyObjekt& obj, Stav& stav) {
  // so far, bossovia su indestructible
  // aby sa mohli hybat hlupo (burat do hviezd)
  //
  log("bossujem");
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
  obj.rychlost= obj.rychlost+kam;
}

void zoznamObjekty(Stav& stav, vector<FyzikalnyObjekt*>& objekty) {
  for (int t=0; t<STAV_TYPOV; t++) {
    for (FyzikalnyObjekt& obj : stav.obj[t]) {
      objekty.push_back(&obj);
    }
  }
  for (Vec& vec : stav.veci) {
    objekty.push_back(&vec.obj);
  }
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    objekty.push_back(&stav.hraci[i].obj);
  }
}

void okamzityEfekt(Stav& stav,const vector<Prikaz>& akcie,const Mapa& mapa) {
  Stav old=stav;
  
  vector<FyzikalnyObjekt*> objekty;
  zoznamObjekty(stav,objekty);
  vector<FyzikalnyObjekt*> stare;
  zoznamObjekty(old,stare);

  // pohni bossmi
  //
  for (FyzikalnyObjekt& boss : stav.obj[konvertDoStavu(BOSS)]) {
    pohniBossom(boss,stav);
  }
  
  // zrazanie objektov
  //
  log("zrazam objekty");
  for (FyzikalnyObjekt* prvy : objekty) {
    vector<double> skore(stav.hraci.size(),0.0);
    for (FyzikalnyObjekt* druhy : stare) {
      if (prvy->id == druhy->id) {
        continue;
      }
      Bod acc = odpal2(*prvy, *druhy);
      Bod antiacc = odpal2(*druhy, *prvy);
      double silaZrazky = acc.dist()+antiacc.dist();
      
      if (! prvy->neznicitelny() ) {
        double damage= druhy->sila*silaZrazky;
        prvy->zivoty -= damage;
        if (druhy->owner>=0 && druhy->owner<(int)skore.size()) {
          skore[druhy->owner]+= damage;
        }
      }
      prvy->rychlost= prvy->rychlost + acc;
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

    udrz(*prvy,mapa);

    // ma na mna vplyv nejaka speci zbran?
    //
    {
      for (int i=0; i<(int)akcie.size(); i++) {
        if (!stav.hraci[i].zije()) {
          continue;
        }
        int pal= akcie[i].pal;
        Bod ciel= akcie[i].ciel;
        
        if (pal==-1) {
          continue;
        }
        int owner= stav.hraci[i].obj.owner;
        Bod poz= stav.hraci[i].obj.pozicia;
        Bod smer= ciel-poz;
        double safedist= stav.hraci[i].obj.polomer + 1.0;
        Bod spawn= poz + smer*(safedist/smer.dist());

        if (pal==VYSTREL_LASER) {
          // laser striela okamzite, prechadza cez vsetko
          // a je relativne slaby
          //
          Bod spojnica= prvy->pozicia-spawn;
          Bod pata= spojnica*smer;
          Bod kolmica= spojnica-pata;
          if (pata/spojnica < 0) {
            continue;
          }
          if (kolmica.dist() >= prvy->polomer) {
            continue;
          }
          prvy->zivoty -= LASER_SILA;
          skore[owner] += LASER_SILA;
        }
      }
    }

    // odmen ostatnych
    //
    for (int j=0; j<(int)skore.size(); j++) {
      if (skore[j]<=0.0) {
        continue;
      }
      if (!prvy->zije()) {
        skore[j]+= kBodyZnic[prvy->typ];
      }
      if (j == prvy->owner) {
        skore[j]*= -1;
      }
      stav.hraci[j].skore+= skore[j];
    }
  }
}

void vypalZoZbrani(Stav& stav,const vector<Prikaz>& akcie,const Mapa& mapa) {
  log("palim zo zbrani");
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
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
    int proj= vystrelNaProj(pal);
    double safedist= polomer+1.0 + (proj!=-INF ? kZ_polomer[pal] : 0.0);
    Bod spawn= poz + smer*(safedist/smer.dist());

    if (proj!=-INF) {
      smer=smer*(kZ_rychlost[pal]/smer.dist());
      Bod p_rychl= rychl+smer;
      FyzikalnyObjekt strela(proj,owner, spawn,p_rychl, kZ_polomer[pal], kZ_koliznyLv[pal], kZ_sila[pal], kZ_zivoty[pal]);
      udrz(strela,mapa);
      stav.obj[PROJ_BEGIN].push_back(strela);
      stav.hraci[i].cooldown= COOLDOWN;
    }
    else {
      if (pal==VYSTREL_LASER) {
        // observujem  |  |
        //             v  v
        obraz laser(stav.hraci[i].obj.owner, spawn,ciel);
        vidim[volne_zap_id]= laser;
        volne_zap_id++;
      }
    }
  }
}

void ziskajPickupy(Stav& stav) {
  // ziskaj pickupy (veci)
  //
  log("pickupy");
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    for (Vec& bonus : stav.veci) {
      if (!zrazka(hrac.obj, bonus.obj)) {
        continue;
      }
      int typ=bonus.obj.typ;
      if (typ < VEC_BEGIN+DRUHOV_ZBRANI) {
        typ= zbranNaVystrel(typ);
        hrac.zbrane[typ]+=bonus.naboje;
      }
      else {
        typ-= VEC_BEGIN+DRUHOV_ZBRANI;
        hrac.veci[typ]+=bonus.naboje;
      }
    }
  }
}

void pohniObjektami(Stav& stav) {
  log("hybem objektami");
  vector<FyzikalnyObjekt*> objekty;
  zoznamObjekty(stav,objekty);
  for (FyzikalnyObjekt* ptr : objekty) {
    ptr->pozicia= ptr->pozicia + ptr->rychlost;
  }
}

void endStep(Stav& stav) {
  // end step
  //
  log("cooldown");
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    if (hrac.cooldown>0) {
      hrac.cooldown--;
    }
  }
  log("vybuchy zniz fazu");
  for (int i=(int)stav.vybuchy.size()-1; i>=0; i--) {
    stav.vybuchy[i].faza--;
    if (stav.vybuchy[i].faza<=0) {
      stav.vybuchy[i]= stav.vybuchy.back();
      stav.vybuchy.pop_back();
    }
  }
  stav.cas++;
}

void pohrebnaSluzba(Stav& stav) {
  // cleanup step -- pochovaj mrtve
  // asteroidy, planety, hviezdy, bossov, PROJ_BEGINy, veci
  //
  log("pochovavam");
  vector<FyzikalnyObjekt> vznikleObjekty;
  vector<Vec> vznikleVeci;
  for (int t=0; t<STAV_TYPOV; t++) {
    for (int i=(int)stav.obj[t].size()-1; i>=0; i--) {
      if (stav.obj[t][i].zije()) {
        continue;
      }
      if (stav.obj[t][i].typ == BOMBA) {
        Vybuch bum(stav.obj[t][i].owner, stav.obj[t][i].pozicia, BUM_POLOMER, BUM_SILA, BUM_TRVANIE);
        stav.vybuchy.push_back(bum);
      }
      rozpad(stav.obj[t][i], vznikleObjekty, vznikleVeci);
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
  for (FyzikalnyObjekt& obj : vznikleObjekty) {
    int t= konvertDoStavu(obj.typ);
    stav.obj[t].push_back(obj);
  }
  for (Vec& item : vznikleVeci) {
    stav.veci.push_back(item);
  }
}

void vrhniAsteroid(Stav& stav, const Mapa& mapa) {
  log("vrham asteroid");
  if (stav.cas % mapa.casAst == 0) {
    int DX[4]={0,1,0,-1};
    int DY[4]={1,0,-1,0};
    int okraj= rand()%4;
    double polomer= AST_MIN_R + rand_float(AST_MAX_R-AST_MIN_R);
    Bod kde(rand_float(mapa.w),rand_float(mapa.h));
    switch (DX[okraj]) {
      case 1: kde.x=mapa.w+polomer; break;
      case -1: kde.x= -polomer;
    }
    switch (DY[okraj]) {
      case 1: kde.y=mapa.h+polomer; break;
      case -1: kde.y= -polomer;
    }
    Bod kam(rand_float(mapa.w),rand_float(mapa.h));
    kam= kam-kde;
    kam= kam*(rand_float(AST_MAX_V)/kam.dist());
    stav.obj[ konvertDoStavu(ASTEROID) ].push_back(vytvorAst(kde,kam,polomer));
  }
}

void odsimuluj(Stav& stav, vector<Prikaz>& akcie, const Mapa& mapa) {
  // axiomy:
  // kazdy objekt musi byt zivy aspon 1 jednotku casu
  // vznikajuce objekty ovplyvnuju a su ovplyvnovane az nasledujucim stavom
  // laser nie je vznikajuci objekt, ten ovplyvnuje aktualny stav

  opravPrikazy(stav,akcie);
  okamzityEfekt(stav,akcie,mapa);
  vypalZoZbrani(stav,akcie,mapa);
  ziskajPickupy(stav);
  pohniObjektami(stav);
  endStep(stav);

  // este pred pochovanim zaznamenaj, lebo umierajuce objekty mozu mat kul animaciu
  log("zaznamenavam");
  zaznamuj(stav);
  pohrebnaSluzba(stav);
  vrhniAsteroid(stav,mapa);
  
  log("vypisujem pre observera");
  if (stav.cas % frame_t == 0) {
    vypis();
  }
}



bool validvMape (int typ) {
  return typ<MAPA_TYPOV && typ>=0;
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
    int typ = konvertDoStavu(mapa.objekty[i].typ);
    if (!validvMape(typ) ) {
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
