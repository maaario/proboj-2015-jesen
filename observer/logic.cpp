#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include "SDL.h"
#include "SDL_ttf.h"
using namespace std;

#include "logic.h"
#include "common.h"
#include "marshal.h"
#include "util.h"

template<class T> void checkStream(T& s, string filename) {
  if (s.fail()) {
    fprintf(stderr, "neviem citat z %s\n", filename.c_str());
    exit(1);
  }
}

void Hra::nacitajSubor(string zaznamovySubor) {
  framy.clear();

  ifstream subor(zaznamovySubor);

  Frame frame;

  string riadok;
  while (getline(subor, riadok)) {
    if (riadok == "") {   // prazdny riadok znamena ukoncenie jedneho framu
      framy.push_back(frame);
      frame.kruhoveObjekty.clear();
      continue;
    }

    stringstream riadokStream(riadok);
    int typ;
    Bod pozicia;
    double polomer;
    riadokStream >> typ >> pozicia.x >> pozicia.y >> polomer;

    frame.kruhoveObjekty.push_back(KruhovyObjekt(typ, pozicia, polomer));
  }
  subor.close();
}

//
// pomocne funkcie na kreslenie jednotlivych objektov
//

void kresliHraca(KruhovyObjekt &objekt, SDL_Surface *surface) {
  //TODO
}

//
// hlavna funkcia na nakreslenie vsetkeho
//

void Frame::kresli(SDL_Surface *surface) {
  for (KruhovyObjekt &objekt: kruhoveObjekty) {
    //TODO
  }
}
