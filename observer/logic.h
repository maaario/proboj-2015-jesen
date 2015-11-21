
#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>

#include "common.h"

struct KruhovyObjekt {
  int typ;
  int majitel;

  Bod zaciatok;
  Bod koniec;
  double polomer;

  int magia;

  KruhovyObjekt(int typ, int majitel, Bod zaciatok, Bod koniec, double polomer) {
    this->typ = typ;
    this->zaciatok = zaciatok;
    this->koniec = koniec;
    this->polomer = polomer;
  }
};

struct Frame {
  vector<KruhovyObjekt> kruhoveObjekty;

  void kresli(SDL_Surface *surface);
};

struct Hra {
  int sirka, vyska;

  vector<Frame> framy;

  void nacitajSubor(string zaznamovySubor);
};

#endif
