#include <fstream>
#include <assert.h>
using namespace std;

#include "mapa.h"
#include "common.h"
#include "marshal.h"
#include "util.h"

fstream in;

void nacitajObjekt(int typ, Mapa& mapa) {
  if (typ==ASTEROID) {
    Bod poz;
    Bod rychl;
    double r;
    assert(in >> poz.x >> poz.y >> rychl.x >> rychl.y >> r);
    double zivoty= AST_ZIV_RATE*r*r;
    FyzickyObjekt obj(typ,-1,poz,rychl,r, AST_SILA, zivoty, AST_KOLIZNY_LV);
    mapa.objekty.push_back(obj);
    return ;
  }
  if (typ==PLANETA) {
    Bod poz;
    Bod rychl;
    double r;
    assert(in >> poz.x >> poz.y >> rychl.x >> rychl.y >> r);
    double zivoty= PLANETA_ZIV_RATE*r*r;
    FyzickyObjekt obj(typ,-1,poz,rychl,r, PLANETA_SILA, zivoty, PLANETA_KOLIZNY_LV);
    mapa.objekty.push_back(obj);
    return ;
  }
  if (typ==BOSS) {
    Bod poz;
    assert(in >> poz.x >> poz.y);
    FyzickyObjekt obj(typ,-1,poz,Bod(),BOSS_POLOMER,BOSS_SILA,BOSS_ZIVOTY,BOSS_KOLIZNY_LV);
    mapa.objekty.push_back(obj);
    return ;
  }
  if (typ==ZLATO) {
    Bod poz;
    Bod rychl;
    assert(in >> poz.x >> poz.y >> rychl.x >> rychl.y);
    FyzickyObjekt obj(typ,-1,poz,rychl,ZLATO_POLOMER,ZLATO_SILA,ZLATO_ZIVOTY,ZLATO_KOLIZNY_LV);
    mapa.objekty.push_back(obj);
    return ;
  }
  log("neznamy objekt vo vesmire");
  assert(false);
}

bool nacitajMapu(Mapa& mapa, string filename) {
  in.open(filename.c_str(), fstream::in);

  assert(in >> mapa.w >> mapa.h);
  assert(in >> mapa.casAst >> mapa.casBoss);
  assert(in >> mapa.astMinR >> mapa.astMaxR >> mapa.astMinVel >> mapa.astMaxVel);
  {
    int n;
    assert(in >> n);
    for (; n>0; n--) {
      Bod kde;
      assert(in >> kde.x >> kde.y);
      mapa.spawny.push_back(kde);
    }
  }
  {
    int n;
    assert(in >> n);
    for (; n>0; n--) {
      int typ;
      assert(in >> typ);
      nacitajObjekt(typ,mapa);
    }
  }

  in.close();
  return true;
}
