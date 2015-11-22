
#ifndef LOGIC_H
#define LOGIC_H

#include <string>
#include <vector>
#include <map>

#include "SDL.h"

#include "common.h"

struct Kamera {
  Bod pozicia;
  double zoom;

  Kamera(Bod pozicia, double zoom) {
    this->pozicia = pozicia;
    this->zoom = zoom;
  }

  Bod transformuj(Bod bod, double sirkaObrazovky, double vyskaObrazovky);
};

struct Obrazky {
  map<string, vector<SDL_Surface*>> obrazky;

  void nacitaj(string meno, string subor);
  void kresli(SDL_Surface *surface, string meno, int index, double x, double y, double polomer);
};

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
  int sirka, vyska;
  vector<KruhovyObjekt> kruhoveObjekty;

  void kresli(SDL_Surface *surface, Kamera &kamera, Obrazky &obrazky);
};

struct Hra {
  int sirka, vyska;
  vector<Frame> framy;

  void nacitajSubor(string zaznamovySubor);
};

#endif
