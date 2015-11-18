#include <fstream>
#include <assert.h>
using namespace std;

#include "mapa.h"
#include "common.h"

void nacitajMapu(Mapa& mapa, string filename) {
  fstream in;
  in.open(filename.c_str(), fstream::in);
  
  in.close();
}
