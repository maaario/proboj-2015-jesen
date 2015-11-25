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

#define PRESNOST 21
#define RANDF_ACC 98765ll
#define SENTINEL_POLOMER 987654.0
#define SAFE_OD_OKRAJA 20.0

static int volne_zap_id = -INF;
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
  double note;

  obraz () {}
  obraz (int t,int own, Bod poz, double r, double nt) :
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
unordered_map<int,obraz> vidim;

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
void vypis(const Stav& stav) {
  if (vidim.empty()) {
    return;
  }
  *g_observation << stav.hraci.size() << "\n";
  for (const Hrac& hrac : stav.hraci) {
    // owner, skore, zivoty,
    // nabojov_puska, nabojov_bomba, nabojov_laser,
    // nabojov_urychl, nabojov_stit, nabojov_lekar
    *g_observation << hrac.obj.owner << " " << hrac.skore << " "
      << hrac.obj.zivoty;
    for (int i=0; i<DRUHOV_ZBRANI; i++) {
      *g_observation << " " << hrac.zbrane[i];
    }
    for (int i=0; i<DRUHOV_VECI; i++) {
      *g_observation << " " << hrac.veci[i];
    }
    *g_observation << "\n";
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






double rand_float (double d) {
  return (double)(rand()%int(1.0 + d*RANDF_ACC))/(double)RANDF_ACC;
}
double rand_float (double l,double r) {
  double d= r-l;
  return l+(double)(rand()%int(1.0 + d*RANDF_ACC))/(double)RANDF_ACC;
}





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

Bod odpal (const FyzikalnyObjekt& A, const FyzikalnyObjekt& B) {
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



Vec vytvorVec(Bod poz,Bod rychl) {
  int typ= rand()%(DRUHOV_ZBRANI+DRUHOV_VECI);
  int nabojov= int(k_nabojov[typ]/2);
  if (nabojov<1) {
    nabojov=1;
  }
  nabojov+= rand()%(1+k_nabojov[typ]-nabojov);
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
    if (rand_float(1.0) < AST_DROP_RATE) {
      vznikleVeci.push_back(vytvorVec(obj.pozicia,obj.rychlost));
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

void vykonajPrikazy(Stav& stav,vector<Prikaz>& akcie) {
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    if (akcie[i].pouzi[POUZI_URYCHLOVAC]>0 && !(akcie[i].acc==Bod()) ) {
      Bod boost= akcie[i].acc*(URYCHLOVAC_SILA/akcie[i].acc.dist());
      stav.hraci[i].obj.okamziteZrychli(boost);
      stav.hraci[i].veci[POUZI_URYCHLOVAC]--;
    }
    if (akcie[i].pouzi[POUZI_STIT]>0) {
      stav.hraci[i].obj.stit+= STIT_TRVANIE;
      stav.hraci[i].veci[POUZI_STIT]--;
    }
    if (akcie[i].pouzi[POUZI_LEKARNICKA]>0) {
      stav.hraci[i].obj.zivoty+= LEKARNICKA_SILA*LOD_ZIVOTY;
      if (stav.hraci[i].obj.zivoty > LOD_ZIVOTY) {
        stav.hraci[i].obj.zivoty = LOD_ZIVOTY;
      }
      stav.hraci[i].veci[POUZI_LEKARNICKA]--;
    }
    stav.hraci[i].obj.zrychli(akcie[i].acc,delta_time);
  }
}

void udrz(FyzikalnyObjekt& obj, const Mapa& mapa) {
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
    FyzikalnyObjekt sentinel(-1,-1,Bod(x,y),Bod(),SENTINEL_POLOMER,INF,SENTINEL_SILA,INF);
    Bod acc= odpal(obj,sentinel);
    obj.okamziteZrychli(acc);
    if (! obj.neznicitelny() ) {
      obj.zivoty -= sentinel.sila*acc.dist();
    }
  }
}

void pohniBossom(FyzikalnyObjekt& obj, Stav& stav) {
  // so far, bossovia su indestructible
  // aby sa mohli hybat hlupo (burat do hviezd)
  //
  Bod kam(obj.pozicia);
  double best= INF;
  for (int i=0; i<(int)stav.hraci.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    double l=0;
    double r=INF;
    Bod smer;
    while (r-l > EPS) {
      double s=(l+r)/2;
      Bod on= stav.hraci[i].obj.pozicia + stav.hraci[i].obj.rychlost*s;
      Bod ja= obj.pozicia + obj.rychlost*s;
      Bod relpoz= on-ja;
      double cas= sqrt(2*relpoz.dist()/BOSS_MAX_ACC);
      if (cas < s) {
        r=s;
        smer= relpoz;
      }
      else {
        l=s;
      }
    }
    if (r < best) {
      best=r;
      kam= smer;
    }
  }
  kam= kam*(BOSS_MAX_ACC/kam.dist());
  obj.zrychli(kam,delta_time);
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
  for (FyzikalnyObjekt* prvy : objekty) {
    vector<double> skore(stav.hraci.size(),0.0);
    for (FyzikalnyObjekt* druhy : stare) {
      if (prvy->id == druhy->id) {
        continue;
      }
      Bod acc = odpal(*prvy, *druhy);
      Bod antiacc = odpal(*druhy, *prvy);
      double silaZrazky = acc.dist()+antiacc.dist();
      
      if (! prvy->neznicitelny() ) {
        double damage= druhy->sila*silaZrazky;
        prvy->zivoty -= damage;
        if (druhy->owner>=0 && druhy->owner<(int)skore.size()) {
          skore[druhy->owner]+= damage;
        }
      }
      prvy->okamziteZrychli(acc);
    }

    // hori? (je vo vybuchu?)
    //
    if (! prvy->neznicitelny() ) {
      for (Vybuch& bum : stav.vybuchy) {
        if (!zrazka(bum,*prvy)) {
          continue;
        }
        double damage= bum.sila*delta_time;
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
          Bod pata= spojnica.pata(smer);
          Bod kolmica= spojnica-pata;
          if (pata/smer < 0) {
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

void vypalZoZbrane(Stav& stav,const vector<Prikaz>& akcie,const Mapa& mapa) {
  for (int i=0; i<(int)akcie.size(); i++) {
    if (!stav.hraci[i].zije()) {
      continue;
    }
    int pal= akcie[i].pal;
    Bod ciel= akcie[i].ciel;
    
    if (pal==-1) {
      continue;
    }
    stav.hraci[i].zbrane[pal]--;
    int owner= stav.hraci[i].obj.owner;
    Bod poz= stav.hraci[i].obj.pozicia;
    Bod rychl= stav.hraci[i].obj.rychlost;
    double polomer= stav.hraci[i].obj.polomer;
    
    Bod smer= ciel-poz;
    int proj= vystrelNaProj(pal);
    double safedist= polomer+1.0 + (proj!=-INF ? k_polomer[pal] : 0.0);
    Bod spawn= poz + smer*(safedist/smer.dist());

    if (proj!=-INF) {
      smer=smer*(k_rychlost[pal]/smer.dist());
      Bod p_rychl= rychl+smer;
      FyzikalnyObjekt strela(proj,owner, spawn,p_rychl, k_polomer[pal], k_koliznyLv[pal], k_sila[pal], k_zivoty[pal]);
      udrz(strela,mapa);
      stav.obj[PROJ_BEGIN].push_back(strela);
      stav.hraci[i].cooldown= COOLDOWN;
    }
    else {
      if (pal==VYSTREL_LASER) {
        stav.hraci[i].cooldown= COOLDOWN;
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
  vector<FyzikalnyObjekt*> objekty;
  zoznamObjekty(stav,objekty);
  for (FyzikalnyObjekt* ptr : objekty) {
    ptr->pohni(delta_time);
  }
}

void endStep(Stav& stav) {
  // end step
  //
  for (Hrac& hrac : stav.hraci) {
    if (!hrac.zije()) {
      continue;
    }
    if (hrac.cooldown>0) {
      hrac.cooldown-= delta_time;
    }
    if (hrac.obj.stit>0) {
      hrac.obj.stit-= delta_time;
    }
  }
  for (int i=(int)stav.vybuchy.size()-1; i>=0; i--) {
    stav.vybuchy[i].faza-= delta_time;
    if (stav.vybuchy[i].faza<=0) {
      stav.vybuchy[i]= stav.vybuchy.back();
      stav.vybuchy.pop_back();
    }
  }
  stav.cas+= delta_time;
}

void pohrebnaSluzba(Stav& stav) {
  // cleanup step -- pochovaj mrtve
  // asteroidy, planety, hviezdy, bossov, PROJ_BEGINy, veci
  //
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
  stav.casAst-= delta_time;
  if (stav.casAst <= 0) {
    stav.casAst= mapa.casAst;
    int okraj= rand()%4;
    double polomer= rand_float(mapa.astMinR,mapa.astMaxR);
    Bod kde= nahodnyNaOkraji(okraj,polomer,mapa);
    double uhol= rand_float(1.25*PII-okraj*0.5*PII, 1.75*PII-okraj*0.5*PII);
    double sila= rand_float(mapa.astMinVel,mapa.astMaxVel);
    Bod rychl(sila*cos(uhol),sila*sin(uhol));
    stav.obj[ konvertDoStavu(ASTEROID) ].push_back(vytvorAst(kde,rychl,polomer));
  }
}

void zavolajBossa(Stav& stav, const Mapa& mapa) {
  stav.casBoss-= delta_time;
  if (stav.casBoss <= 0) {
    stav.casBoss= BOSS_PERIODA;
    int okraj= rand()%4;
    Bod spawn= nahodnyNaOkraji(okraj,BOSS_POLOMER,mapa);
    FyzikalnyObjekt boss(BOSS,-1, spawn,Bod(),BOSS_POLOMER, BOSS_KOLIZNY_LV,
      BOSS_SILA,BOSS_ZIVOTY);
    stav.obj[ konvertDoStavu(BOSS) ].push_back(boss);
  }
}
  

void odsimuluj(Stav& stav, vector<Prikaz>& akcie, const Mapa& mapa, double dt) {
  // axiomy:
  // kazdy objekt musi byt zivy aspon 1 jednotku casu
  // vznikajuce objekty ovplyvnuju a su ovplyvnovane az nasledujucim stavom
  // laser nie je vznikajuci objekt, ten ovplyvnuje aktualny stav
  delta_time = dt;

  opravPrikazy(stav,akcie);
  vykonajPrikazy(stav,akcie);
  okamzityEfekt(stav,akcie,mapa);
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



bool validvMape (int typ) {
  return typ<MAPA_TYPOV && typ>=0;
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
