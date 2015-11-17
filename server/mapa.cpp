#include <fstream>
#include <assert.h>
using namespace std;

#include "mapa.h"
#include "common.h"

void nacitajMapu(Mapa& mapa, string filename) {
  mapa.spawny.clear();
  mapa.planety.clear();
  
  fstream in;
  in.open(filename.c_str(), fstream::in);

  //nacitaj spawny
  {
    int n;
    assert(in >> n);
    for (; n>0; n--) {
      double x,y;
      assert(in >> x >> y);
      mapa.spawny.push_back(Spawn(x,y));
    }
  }

  //nacitaj planety
  {
    int n;
    assert(in >> n);
    for (; n>0; n--) {
      double x,y, r;
      assert(in >> x >> y >> r);
      mapa.planety.push_back(Planeta(x,y,r));
    }
  }

  in.close();
}
