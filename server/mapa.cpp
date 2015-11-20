#include <fstream>
using namespace std;

#include "mapa.h"
#include "common.h"
#include "marshal.h"
#include "util.h"

bool nacitajMapu(Mapa& mapa, string filename) {
  fstream in;
  in.open(filename.c_str(), fstream::in);

  nacitaj(in,mapa);
  if (mapa.w < 0 || mapa.h < 0 || mapa.spawny.size()==0) {
    // stale sa ale moze stat, ze sa ju nepodarilo celkom nacitat...
    log("nepodarilo sa nacitat mapu");
    return false;
  }

  in.close();
  return true;
}
