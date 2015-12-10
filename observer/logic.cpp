#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_rotozoom.h"
using namespace std;

#include "logic.h"
#include "common.h"
#include "marshal.h"
#include "util.h"

const double RED[]    = { 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5 };
const double GREEN[]  = { 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.5, 0.5, 1.0, 1.0, 0.5 };
const double BLUE[]   = { 0.0, 1.0, 0.0, 1.0, 0.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0 };

template<class T> void checkStream(T& s, string filename) {
  if (s.fail()) {
    fprintf(stderr, "neviem citat z %s\n", filename.c_str());
    exit(1);
  }
}

Bod Kamera::transformuj(Bod bod, double sirkaObrazovky, double vyskaObrazovky, Bod offset) {
  bod = bod - pozicia - offset;
  bod = bod * zoom;
  bod.x += sirkaObrazovky / 2 + offset.x / 2;
  bod.y += vyskaObrazovky / 2 + offset.y / 2;
  bod = bod + offset;
  return bod;
}

void Obrazky::nacitaj(string meno, string subor) {
  SDL_Surface *surovy_obrazok = IMG_Load(subor.c_str());
  if (surovy_obrazok == NULL) {
    fprintf(stderr, "nepodarilo sa nacitat obrazok \"%s\"\n", subor.c_str());
    exit(1);
  }

  SDL_Surface *konvertovany_obrazok = SDL_DisplayFormatAlpha(surovy_obrazok);
  SDL_FreeSurface(surovy_obrazok);
  if (konvertovany_obrazok == NULL) {
    fprintf(stderr, "nepodarilo sa skonvertovat obrazok \"%s\"\n", subor.c_str());
    exit(1);
  }

  obrazky[meno].push_back(konvertovany_obrazok);
}

void Obrazky::kresli(SDL_Surface *surface, string meno, int index, double x, double y, double polomer) {
  int x_int = round(x - polomer);
  int y_int = round(y - polomer);
  int priemer_int = round(polomer);
  SDL_Rect dstrect;
  dstrect.x = x_int;
  dstrect.y = y_int;
  dstrect.w = priemer_int;
  dstrect.h = priemer_int;

  SDL_Surface *obrazok = obrazky[meno][index];

  double pomer_x = 2 * (double)priemer_int / (double)obrazok->w;
  double pomer_y = 2 * (double)priemer_int / (double)obrazok->h;
  SDL_Surface *natiahnuty = zoomSurface(obrazok, pomer_x, pomer_y, SMOOTHING_OFF);
  SDL_BlitSurface(natiahnuty, NULL, surface, &dstrect);
  SDL_FreeSurface(natiahnuty);
}

void Hra::nacitajSubor(string zaznamovySubor) {
  framy.clear();

  ifstream subor(zaznamovySubor);
  stringstream riadokStream;

  string riadok;

  //
  // nacitame mena hracov
  //

  getline(subor, riadok);
  riadokStream.clear();
  riadokStream << riadok;

  int pocetHracov;
  riadokStream >> pocetHracov;

  for (int i = 0; i < pocetHracov; ++i) {
    getline(subor, riadok);
    riadokStream.clear();
    riadokStream << riadok;

    string meno;
    riadokStream >> meno;

    menaHracov.push_back(meno);
  }

  //
  // nacitame rozmery mapy
  //

  getline(subor, riadok);
  riadokStream.clear();
  riadokStream << riadok;

  riadokStream >> sirka >> vyska;

  Frame frame;
  frame.menaHracov = menaHracov;
  frame.sirka = sirka;
  frame.vyska = vyska;

  getline(subor, riadok); // prazdny riadok na oddelenie headeru

  //
  // nacitanie jednotlivych framov
  //

  while (true) {
    if (!getline(subor, riadok)) break;
    riadokStream.clear();
    riadokStream << riadok;

    int pocetHracov;
    riadokStream >> pocetHracov;

    for (int i = 0; i < pocetHracov; ++i) {
      getline(subor, riadok);
      riadokStream.clear();
      riadokStream << riadok;

      StavHraca sh;
      riadokStream >> sh.majitel >> sh.skore >> sh.zivoty >> sh.zasobnik;

      frame.stavyHracov.push_back(sh);
    }

    while (true) {
      getline(subor, riadok);
      if (riadok == "") {   // prazdny riadok znamena ukoncenie jedneho framu
        framy.push_back(frame);
        frame.stavyHracov.clear();
        frame.kruhoveObjekty.clear();
        break;
      }
      riadokStream.clear();
      riadokStream << riadok;

      KruhovyObjekt ko;
      riadokStream >> ko.typ >> ko.majitel >> ko.zaciatok.x >> ko.zaciatok.y;
      riadokStream >> ko.koniec.x >> ko.koniec.y >> ko.polomer >> ko.magia;

      frame.kruhoveObjekty.push_back(ko);
    }
  }
  subor.close();
}

//
// pomocne funkcie na kreslenie jednotlivych objektov
//

void kresliKruh(SDL_Surface *surface, double x, double y, double polomer, double r, double g, double b, double a) {
  int x_int = round(x);
  int y_int = round(y);
  int polomer_int = round(polomer);
  int r_int = round(255 * r);
  int g_int = round(255 * g);
  int b_int = round(255 * b);
  int a_int = round(255 * a);
  filledEllipseRGBA(surface, x_int, y_int, polomer_int, polomer_int, r_int, g_int, b_int, a_int);
}

void kresliAsteroid(SDL_Surface *surface, KruhovyObjekt &objekt) {
  kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, 0.8, 0.8, 0.8, 1.0);
}

void kresliPlanetu(SDL_Surface *surface, KruhovyObjekt &objekt) {
  kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, 0.4, 0.2, 0.2, 1.0);
}

void kresliLod(SDL_Surface *surface, KruhovyObjekt &objekt) {
  double r = RED[objekt.majitel];
  double g = GREEN[objekt.majitel];
  double b = BLUE[objekt.majitel];
  kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, r, g, b, 1.0);
}

void kresliZlato(SDL_Surface *surface, KruhovyObjekt &objekt) {
  kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, 1.0, 1.0, 0.0, 1.0);
}

void kresliLaser(SDL_Surface *surface, KruhovyObjekt &objekt) {
  int x1 = round(objekt.zaciatok.x);
  int y1 = round(objekt.zaciatok.y);
  int x2 = round(objekt.koniec.x);
  int y2 = round(objekt.koniec.y);
  thickLineRGBA(surface, x1, y1, x2, y2, 2, 0.0, 0.0, 1.0, 1.0);
}

//
// hlavna funkcia na nakreslenie vsetkeho
//

void Frame::kresli(SDL_Surface *surface, Kamera &kamera, Obrazky &obrazky, TTF_Font *font) {
  Bod offset(0, 0);

  //
  // nakreslime hernu plochu
  //

  // nakreslime okraje mapy
  Bod lavyHorny = kamera.transformuj(Bod(0, 0), surface->w, surface->h, offset);
  Bod pravyHorny = kamera.transformuj(Bod(sirka, 0), surface->w, surface->h, offset);
  Bod pravyDolny = kamera.transformuj(Bod(sirka, vyska), surface->w, surface->h, offset);
  Bod lavyDolny = kamera.transformuj(Bod(0, vyska), surface->w, surface->h, offset);

  thickLineRGBA(surface, round(lavyHorny.x), round(lavyHorny.y),
                         round(pravyHorny.x), round(pravyHorny.y),
                1, 255, 255, 255, 255);
  thickLineRGBA(surface, round(pravyHorny.x), round(pravyHorny.y),
                         round(pravyDolny.x), round(pravyDolny.y),
                1, 255, 255, 255, 255);
  thickLineRGBA(surface, round(pravyDolny.x), round(pravyDolny.y),
                         round(lavyDolny.x), round(lavyDolny.y),
                1, 255, 255, 255, 255);
  thickLineRGBA(surface, round(lavyDolny.x), round(lavyDolny.y),
                         round(lavyHorny.x), round(lavyHorny.y),
                1, 255, 255, 255, 255);

  for (KruhovyObjekt &objekt: kruhoveObjekty) {
    KruhovyObjekt podlaKamery;
    podlaKamery.typ = objekt.typ;
    podlaKamery.majitel = objekt.majitel;
    podlaKamery.zaciatok = kamera.transformuj(objekt.zaciatok, surface->w, surface->h, offset);
    podlaKamery.koniec = kamera.transformuj(objekt.koniec, surface->w, surface->h, offset);
    podlaKamery.polomer = objekt.polomer * kamera.zoom;
    podlaKamery.magia = objekt.magia;

    switch (objekt.typ) {
      case ASTEROID: {
        obrazky.kresli(surface, "asteroid", 0, podlaKamery.koniec.x, podlaKamery.koniec.y, podlaKamery.polomer);
      } break;

      case PLANETA: {
        //kresliPlanetu(surface, podlaKamery);
        obrazky.kresli(surface, "earth", 0, podlaKamery.koniec.x, podlaKamery.koniec.y, podlaKamery.polomer);
      } break;

      case LOD: {
        kresliLod(surface, podlaKamery);
      } break;

      case ZLATO: {
        kresliZlato(surface, podlaKamery);
      } break;

      case BOSS: {
				obrazky.kresli(surface, "troll", 0, podlaKamery.koniec.x, podlaKamery.koniec.y, podlaKamery.polomer);
			} break;

      default: {
        kresliKruh(surface, podlaKamery.koniec.x, podlaKamery.koniec.y, podlaKamery.polomer, 1, 0, 0, 1);
      } break;
    }
  }

  //
  // nakreslime informacie o hracoch
  //

  for (int i = 0; i < stavyHracov.size(); ++i) {
    double x = (double)surface->w / (double)stavyHracov.size() * i;
    double y = 0;
    double w = (double)surface->w / (double)stavyHracov.size();
    double h = 80;

    SDL_Color fg;
    fg.r = round(RED[i] * 255);
    fg.g = round(GREEN[i] * 255);
    fg.b = round(BLUE[i] * 255);

    SDL_Color bg;
    bg.r = 0;
    bg.g = 0;
    bg.b = 0;

    string text;
    SDL_Surface *rendered;
    SDL_Rect dst;

    text = menaHracov[i];
    rendered = TTF_RenderUTF8_Shaded(font, text.c_str(), fg, bg);
    dst.x = round(x + w / 8);
    dst.y = round(y + h / 8);
    SDL_BlitSurface(rendered, NULL, surface, &dst);
    SDL_FreeSurface(rendered);

    text = to_string(stavyHracov[i].skore);
    rendered = TTF_RenderUTF8_Shaded(font, text.c_str(), fg, bg);
    dst.x = round(x + w / 8);
    dst.y = round(y + h / 8 + h / 4);
    SDL_BlitSurface(rendered, NULL, surface, &dst);
    SDL_FreeSurface(rendered);

    if (stavyHracov[i].zivoty <= 0) {
      text = "DEAD";
      rendered = TTF_RenderUTF8_Shaded(font, text.c_str(), fg, bg);
      dst.x = round(x + w / 8);
      dst.y = round(y + h / 8 + h / 4 + h / 4);
      SDL_BlitSurface(rendered, NULL, surface, &dst);
      SDL_FreeSurface(rendered);
    } else {
      int bar_x = round(x + w / 8);
      int bar_y = round(y + h / 8 + h / 4 + h / 4);
      int bar_w = round(stavyHracov[i].zivoty / LOD_ZIVOTY * (w - 2 * w / 8));
      int bar_h = round(h / 4);
      boxRGBA(surface, bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, fg.r, fg.g, fg.b, 255);
    }
  }
}
