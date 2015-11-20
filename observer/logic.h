
#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>

#include "common.h"

struct KruhovyObjekt {
  int typ;

  Bod pozicia;
  double polomer;

  KruhovyObjekt(int typ, Bod pozicia, double polomer) {
    this->typ = typ;
    this->pozicia = pozicia;
    this->polomer = polomer;
  }
};

struct Frame {
  vector<KruhovyObjekt> kruhoveObjekty;

  void kresli(SDL_Surface *surface);
};

struct Hra {
  vector<Frame> framy;

  void nacitajSubor(string zaznamovySubor);
};

#endif
