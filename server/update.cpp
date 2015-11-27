#include <iostream>
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

#define PRESNOST 1000
#define RANDF_ACC 98765ll
#define SENTINEL_POLOMER 987654.0
#define SAFE_OD_OKRAJA 20.0

static double delta_time = delta_time;

static double casFrame;
static double frame_t;
static ostream* g_observation;

const static int DX[4]={0,1,0,-1};
const static int DY[4]={1,0,-1,0};


void zapniObservation(ostream* observation, double ft) {
  g_observation = observation;
  frame_t = ft;
  casFrame = ft;
}
struct obraz {
  int typ;
  int owner;
  Bod zac,kon;
  double polomer;
  bool zije;

  obraz () {}
  obraz (int t,int own, Bod poz, double r, bool lives) :
    typ(t), owner(own), zac(poz), kon(poz), polomer(r), zije(lives) {}
  obraz (const FyzickyObjekt& obj) {
    typ=obj.typ;
    owner=obj.owner;
    zac=obj.pozicia;
    kon=obj.pozicia;
    polomer=obj.polomer;
    zije= obj.zije();
  }
};
unordered_map<int,obraz> vidim;

void zazrel(FyzickyObjekt& obj) {
  if (vidim.count(obj.id)) {
    vidim[obj.id].kon= obj.pozicia;
    if (!obj.zije()) {
      vidim[obj.id].zije= false;
    }
  }
  else {
    vidim[obj.id]= obraz(obj);
  }
}
void zaznamuj(Stav& stav) {
  for (int t=0; t<NORM_TYPOV; t++) {
    for (FyzickyObjekt& obj : stav.obj[t]) {
      zazrel(obj);
    }
  }
  for (Hrac& hrac : stav.hraci) {
    if (hrac.obj.zivoty <= -INF) {
      continue;
    }
    zazrel(hrac.obj);
  }
}
void vypis(const Stav& stav) {
  if (vidim.size() == 0) {
    return;
  }
  *g_observation << stav.hraci.size() << "\n";
  for (const Hrac& hrac : stav.hraci) {
    // owner, skore, zivoty, zasobnik, zasobnikCooldown, cooldown
    *g_observation << hrac.obj.owner << " " << hrac.skore << " "
      << hrac.obj.zivoty << " " << hrac.zasobnik << "\n";
  }
  for (pair<const int,obraz>& parik : vidim) {
    obraz* ptr= &parik.second;
    *g_observation << ptr->typ << " " << ptr->owner << " " << ptr->zac.x
      << " " << ptr->zac.y << " " << ptr->kon.x << " " << ptr->kon.y
      << " " << ptr->polomer << " " << ptr->zije << "\n";
  }
  *g_observation << endl;
  vidim.clear();
}






double rand_float (double d) {
  return (double)(rand()%int(1.0 + d*RANDF_ACC))/(double)RANDF_ACC;
}
double rand_float (double l,double r) {
  double d= r-l;
  return l+(double)(rand()%int(1.0 + d*RANDF_ACC))/(double)RANDF_ACC;
}





Bod nahodnyNaOkraji (int okraj,double polomer, const Mapa& mapa) {
  Bod kde(rand_float(mapa.w),rand_float(mapa.h));
  switch (DX[okraj]) {
    case 1: kde.x=mapa.w+polomer+SAFE_OD_OKRAJA; break;
    case -1: kde.x= -polomer-SAFE_OD_OKRAJA;
  }
  switch (DY[okraj]) {
    case 1: kde.y=mapa.h+polomer+SAFE_OD_OKRAJA; break;
    case -1: kde.y= -polomer-SAFE_OD_OKRAJA;
  }
  return kde;
}





template<class T1, class T2>
bool zrazka (const T1& A, const T2& B) {
  Bod spojnica = A.pozicia - B.pozicia;
  return A.polomer + B.polomer > spojnica.dist();
}

double casDoZraz (Bod relpoz, Bod relvel, double polomer) {
  // relpoz == jeho pozicia relativne ku mne
  // relvel == moja rychlost relativne k nemu
  // polomer == dokopy polomery zrazajucich objektov
  //
  if (relpoz.dist() < polomer) {
    return 0;
  }
  Bod pata= relpoz.pata(relvel);
  Bod kolm= relpoz - pata;
  if (kolm.dist() >= polomer) {
    if (relpoz/relvel <= 0) {
      return -INF;
    }
    return INF;
  }
  double polodvesna= polomer*polomer - kolm.dist2();
  if (polodvesna < 0) {
    polodvesna= 0;
  }
  else {
    polodvesna= sqrt(polodvesna);
  }
  Bod zraz= pata*((pata.dist()-polodvesna)/pata.dist());
  return zraz/relvel;
}

Bod odpal (const FyzickyObjekt& A, const FyzickyObjekt& B) {
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
  projekcia= projekcia.pata(spojnica);

  Bod vysl= (spojnica-projekcia)*cast;
  if (vysl/spojnica < 0) {
    return Bod();
  }
  return vysl;
}



FyzickyObjekt vytvorVec(Bod poz,Bod rychl) {
  FyzickyObjekt res(ZLATO,-1, poz,rychl,ZLATO_POLOMER, ZLATO_SILA, ZLATO_ZIVOTY,ZLATO_KOLIZNY_LV);
  return res;
}

FyzickyObjekt vytvorAst(Bod poz,Bod rychl, double r) {
  double zivoty= AST_ZIV_RATE*r*r;
  FyzickyObjekt res(ASTEROID,-1, poz,rychl,r, AST_SILA, zivoty,AST_KOLIZNY_LV);
  return res;
}

void rozpad(const FyzickyObjekt& obj, vector<FyzickyObjekt>& vznikleObjekty) {
  if (obj.typ!=ASTEROID && obj.typ!=PLANETA) {
    return;
  }
  
  if (obj.polomer/2 < AST_MIN_R) {
    if (rand_float(1.0) < AST_DROP_RATE) {
      vznikleObjekty.push_back(vytvorVec(obj.pozicia,obj.rychlost));
    }
    return;
  }
  vznikleObjekty.push_back(vytvorAst(obj.pozicia,obj.rychlost,obj.polomer/2));
  
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
      pozicia= obj.pozicia+pozicia;

      bool jeVec= (rand_float(1.0) < AST_DROP_RATE);
      pozy.push_back(make_pair(jeVec,pozicia));
      
      int obsadenyUsek= PRESNOST;
      if (jeVec) {
        obsadenyUsek*= ZLATO_POLOMER;
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
    /*
    double uhol= rand_float(2.0*PII);
    Bod rychl(acc*sin(uhol),acc*cos(uhol));
    */
    Bod rychl= poz-obj.pozicia;
    rychl= rychl*(acc/rychl.dist());
    rychl= obj.rychlost+rychl;
    bool jeVec= pozy[i].first;
    if (jeVec) {
      vznikleObjekty.push_back(vytvorVec(poz,rychl));
    }
    else {
      if (obj.polomer/4 >= AST_MIN_R) {
        vznikleObjekty.push_back(vytvorAst(poz,rychl,obj.polomer/4));
      }
    }
  }
}

void opravPrikazy(Stav& stav,vector<Prikaz>& akcie) {
  // uprav prikazy klientov:
  // aby neakcelerovali viac ako LOD_MAX_ACC (nasledne hned upravim rychlost hracov)
  // aby pouzivali/strielali len to, coho naboje maju
  //
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    double pomer=akcie[i].acc.dist()/LOD_MAX_ACC;
    if (pomer>1) {
      akcie[i].acc=akcie[i].acc*(1.0/pomer);
    }
    if (stav.hraci[i].zasobnik==0 || stav.hraci[i].cooldown>0) {
      akcie[i].ciel=Bod();
    }
  }
}

void vykonajPrikazy(Stav& stav,vector<Prikaz>& akcie) {
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    stav.hraci[i].obj.zrychli(akcie[i].acc,delta_time);
  }
}

void udrz(FyzickyObjekt& obj, const Mapa& mapa) {
  // udrz objekty v hracom poli
  // aby platilo: kazdy objekt mimo mapy sa nevie hybat dalej od mapy
  //
  for (int smer=0; smer<4; smer++) {
    double x= obj.pozicia.x;
    double y= obj.pozicia.y;
    switch (DX[smer]) {
      case 1: x=mapa.w+SENTINEL_POLOMER; break;
      case -1: x= -SENTINEL_POLOMER;
    }
    switch (DY[smer]) {
      case 1: y=mapa.h+SENTINEL_POLOMER; break;
      case -1: y= -SENTINEL_POLOMER;
    }
    FyzickyObjekt sentinel(-1,-1,Bod(x,y),Bod(),SENTINEL_POLOMER,SENTINEL_SILA,INF,INF);
    Bod acc= odpal(obj,sentinel);
    obj.okamziteZrychli(acc);
    obj.zivoty -= sentinel.sila*acc.dist();
  }
}

void pohniBossom(FyzickyObjekt& obj, Stav& stav) {
  // so far, bossovia su indestructible
  // aby sa mohli hybat hlupo (burat do hviezd)
  //
  Bod kam;
  double best= INF;
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    Bod relpoz= stav.hraci[i].obj.pozicia - obj.pozicia;
    Bod relvel= obj.rychlost - stav.hraci[i].obj.rychlost;
    Bod pata= relpoz.pata(relvel);
    double dolina= pata/relvel;
    double L[2]={ max(0.0,dolina), 0 };
    double R[2]={ INF, dolina };
    for (int t=0; t<2; t++) {
      double l= L[t];
      double r= R[t];
      bool vyhovuje=false;
      Bod smer;
      while (r-l > EPS) {
        double s=(l+r)/2;
        Bod kde= relpoz + relvel*s;
        double cas= sqrt(2*kde.dist()/BOSS_MAX_ACC);
        if (cas < s) {
          vyhovuje=true;
          r=s;
          smer= relpoz;
        }
        else {
          l=s;
        }
      }
      if (r < best && vyhovuje) {
        best=r;
        kam= smer;
      }
    }
  }
  kam= kam*(BOSS_MAX_ACC/kam.dist());
  obj.zrychli(kam,delta_time);
}

void zoznamObjekty(Stav& stav, vector<FyzickyObjekt*>& objekty) {
  for (int t=0; t<NORM_TYPOV; t++) {
    for (FyzickyObjekt& obj : stav.obj[t]) {
      objekty.push_back(&obj);
    }
  }
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    objekty.push_back(&stav.hraci[i].obj);
  }
}

void okamzityEfekt(Stav& stav,const Mapa& mapa) {
  Stav old=stav;
  
  vector<FyzickyObjekt*> objekty;
  zoznamObjekty(stav,objekty);
  vector<FyzickyObjekt*> stare;
  zoznamObjekty(old,stare);

  // pohni bossmi
  //
  for (FyzickyObjekt& boss : stav.obj[BOSS]) {
    pohniBossom(boss,stav);
  }
  
  // zrazanie objektov
  //
  for (FyzickyObjekt* prvy : objekty) {
    vector<bool> trafil(stav.hraci.size(),false);
    for (FyzickyObjekt* druhy : stare) {
      if (!zrazka(*prvy,*druhy) || prvy->id == druhy->id) {
        continue;
      }
      Bod acc = odpal(*prvy, *druhy);
      Bod antiacc = odpal(*druhy, *prvy);
      double silaZrazky = acc.dist()+antiacc.dist();
      
      double damage= druhy->sila*silaZrazky;
      prvy->zivoty -= damage;
      if (druhy->owner != -1) {
        trafil[druhy->owner]= true;
      }
      prvy->okamziteZrychli(acc);
    }

    udrz(*prvy,mapa);
    
    // odmen ostatnych
    //
    for (int j=0; j<(int)trafil.size(); j++) {
      if (!trafil[j] || prvy->zije()) {
        continue;
      }
      if (j == prvy->owner) {
        stav.hraci[j].skore-= kSkoreZnic[prvy->typ];
      }
      else {
        stav.hraci[j].skore+= kSkoreZnic[prvy->typ];
      }
    }
  }
}

void vypalZoZbrane(Stav& stav,const vector<Prikaz>& akcie,const Mapa& mapa) {
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    Bod ciel= akcie[i].ciel;
    
    if (ciel==Bod()) {
      continue;
    }
    ciel= stav.hraci[i].obj.pozicia + ciel;
    stav.hraci[i].zasobnik--;
    int owner= stav.hraci[i].obj.owner;
    Bod poz= stav.hraci[i].obj.pozicia;
    Bod rychl= stav.hraci[i].obj.rychlost;
    double polomer= stav.hraci[i].obj.polomer;
    
    Bod smer= ciel-poz;
    double safedist= polomer+1.0+STRELA_POLOMER;
    Bod spawn= poz + smer*(safedist/smer.dist());

    smer=smer*(STRELA_RYCHLOST/smer.dist());
    Bod p_rychl= rychl+smer;
    FyzickyObjekt strela(STRELA,owner, spawn,p_rychl, STRELA_POLOMER, STRELA_SILA, STRELA_ZIVOTY,STRELA_KOLIZNY_LV);
    udrz(strela,mapa);
    stav.obj[STRELA].push_back(strela);
    stav.hraci[i].cooldown= COOLDOWN;
  }
}

void ziskajPickupy(Stav& stav) {
  // ziskaj pickupy (veci)
  //
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    for (FyzickyObjekt& bonus : stav.obj[ZLATO]) {
      if (!zrazka(hrac.obj, bonus)) {
        continue;
      }
      hrac.skore+= ZLATO_HODNOTA;
      bonus.zivoty= -INF;
    }
  }
}

void pohniObjektami(Stav& stav) {
  vector<FyzickyObjekt*> objekty;
  zoznamObjekty(stav,objekty);
  for (FyzickyObjekt* ptr : objekty) {
    ptr->pohni(delta_time);
  }
}

void endStep(Stav& stav) {
  // end step
  //
  if (stav.zivychHracov() <= 1) {
    for (Hrac& hrac : stav.hraci) {
      if (hrac.zije()) {
        hrac.skore+= SKORE_ZA_PREZITIE;
      }
    }
  }
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    if (hrac.cooldown>0) {
      hrac.cooldown-= delta_time;
    }
    if (hrac.zasobnik<ZASOBNIK) {
      if (hrac.zasobnikCooldown<=0) {
        hrac.zasobnikCooldown= DODAVACIA_DOBA;
        hrac.zasobnik++;
      }
      else {
        hrac.zasobnikCooldown-= delta_time;
      }
    }
  }
  stav.cas+= delta_time;
}

void pohrebnaSluzba(Stav& stav) {
  // cleanup step -- pochovaj mrtve
  // asteroidy, planety, hviezdy, bossov, PROJ_BEGINy, veci
  //
  vector<FyzickyObjekt> vznikleObjekty;
  for (int t=0; t<NORM_TYPOV; t++) {
    for (int i=(int)stav.obj[t].size()-1; i>=0; i--) {
      if (stav.obj[t][i].zije()) {
        continue;
      }
      rozpad(stav.obj[t][i], vznikleObjekty);
      stav.obj[t][i]= stav.obj[t].back();
      stav.obj[t].pop_back();
    }
  }
  for (Hrac& hrac : stav.hraci) {
    if (hrac.zije()) {
      continue;
    }
    hrac.obj.zivoty= -INF;
  }
  for (FyzickyObjekt& obj : vznikleObjekty) {
    int t= obj.typ;
    stav.obj[t].push_back(obj);
  }
}

void vrhniAsteroid(Stav& stav, const Mapa& mapa) {
  stav.casAst-= delta_time;
  if (stav.casAst <= 0) {
    stav.casAst= mapa.casAst;
    int okraj= rand()%4;
    double polomer= rand_float(mapa.astMinR,mapa.astMaxR);
    Bod kde= nahodnyNaOkraji(okraj,polomer,mapa);
    double uhol= rand_float(1.25*PII-okraj*0.5*PII, 1.75*PII-okraj*0.5*PII);
    double sila= rand_float(mapa.astMinVel,mapa.astMaxVel);
    Bod rychl(sila*cos(uhol),sila*sin(uhol));
    stav.obj[ASTEROID].push_back(vytvorAst(kde,rychl,polomer));
  }
}

void zavolajBossa(Stav& stav, const Mapa& mapa) {
  stav.casBoss-= delta_time;
  if (stav.casBoss <= 0) {
    stav.casBoss= BOSS_PERIODA;
    int okraj= rand()%4;
    Bod spawn= nahodnyNaOkraji(okraj,BOSS_POLOMER,mapa);
    FyzickyObjekt boss(BOSS,-1, spawn,Bod(),BOSS_POLOMER,BOSS_SILA,BOSS_ZIVOTY,BOSS_KOLIZNY_LV);
    stav.obj[BOSS].push_back(boss);
  }
}


void odsimuluj(Stav& stav, vector<Prikaz>& akcie, const Mapa& mapa, double dt) {
  // odsimuluje 1 krok hry na cas dt (cim vacsi cas, tym mensia presnost !!!)
  // ==> odsimulovat ten isty cas na viac krokov je presnejsie
  //

  delta_time = dt;

  opravPrikazy(stav,akcie);
  vykonajPrikazy(stav,akcie);
  okamzityEfekt(stav,mapa);
  vypalZoZbrane(stav,akcie,mapa);
  ziskajPickupy(stav);
  endStep(stav);
  
  zaznamuj(stav);
  
  pohrebnaSluzba(stav);
  pohniObjektami(stav);
  vrhniAsteroid(stav,mapa);
  zavolajBossa(stav,mapa);
  
  casFrame-= delta_time;
  if (casFrame <= 0) {
    casFrame= frame_t;
    vypis(stav);
  }
}





bool pociatocnyStav(Mapa& mapa, Stav& stav, int pocKlientov) {
  stav.cas = 0.0;
  stav.casAst= mapa.casAst;
  stav.casBoss= mapa.casBoss;

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
    FyzickyObjekt K= mapa.objekty[i];
    FyzickyObjekt obj(K.typ,K.owner,K.pozicia,K.rychlost,K.polomer,K.sila,K.zivoty,K.koliznyLevel); //treba nastavit korektne id
    stav.obj[typ].push_back(obj);
  }
  mapa.objekty.clear();

  return true;
}
