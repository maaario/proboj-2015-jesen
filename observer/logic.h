
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

  Bod transformuj(Bod bod, double sirkaObrazovky, double vyskaObrazovky, Bod offset);
};

struct Obrazky {
  map<string, vector<SDL_Surface*>> obrazky;

  void nacitaj(string meno, string subor);
  void kresli(SDL_Surface *surface, string meno, int index, double x, double y, double polomer);
};

struct StavHraca {
  int majitel;

  double skore, zivoty;
  int zasobnik;
};

struct KruhovyObjekt {
  int typ;
  int majitel;

  Bod zaciatok;
  Bod koniec;
  double polomer;

  int magia;
};

struct Frame {
  vector<string> menaHracov;

  int sirka, vyska;
  vector<StavHraca> stavyHracov;
  vector<KruhovyObjekt> kruhoveObjekty;

  void kresli(SDL_Surface *surface, Kamera &kamera, Obrazky &obrazky, TTF_Font *font);
};

struct Hra {
  vector<string> menaHracov;

  int sirka, vyska;
  vector<Frame> framy;

  void nacitajSubor(string zaznamovySubor);
};

#endif
