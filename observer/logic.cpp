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

template<class T> void checkStream(T& s, string filename) {
  if (s.fail()) {
    fprintf(stderr, "neviem citat z %s\n", filename.c_str());
    exit(1);
  }
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

  string riadok;

  getline(subor, riadok);
  stringstream riadokStream(riadok);
  riadokStream >> sirka >> vyska;
  getline(subor, riadok);

  Frame frame;

  while (getline(subor, riadok)) {
    if (riadok == "") {   // prazdny riadok znamena ukoncenie jedneho framu
      framy.push_back(frame);
      frame.kruhoveObjekty.clear();
      continue;
    }

    stringstream riadokStream(riadok);
    int typ;
    int majitel;
    Bod zaciatok, koniec;
    double polomer;
    int magia;
    riadokStream >> typ >> majitel >> zaciatok.x >> zaciatok.y >> koniec.x >> koniec.y >> polomer >> magia;

    frame.kruhoveObjekty.push_back(KruhovyObjekt(typ, majitel, zaciatok, koniec, polomer));
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

void kresliLod(SDL_Surface *surface, KruhovyObjekt &objekt) {
  double r1 = 0.4, g1 = 0.4, b1 = 0.8;
  double r2 = 0.2, g2 = 0.2, b2 = 0.9;
  for (double i = 0; i < 10; ++i) {
    double r = r1 + (r2 - r1) * (i / 10);
    double g = g1 + (g2 - g1) * (i / 10);
    double b = b1 + (b2 - b1) * (i / 10);
    double polomer = objekt.polomer * (10 - i) / 10;
    kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, polomer, r, g, b, 1.0);
  }
}

void kresliVec(SDL_Surface *surface, KruhovyObjekt &objekt) {
  kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, 1.0, 1.0, 0.0, 1.0);
}

//
// hlavna funkcia na nakreslenie vsetkeho
//

void Frame::kresli(SDL_Surface *surface, Obrazky &obrazky) {
  for (KruhovyObjekt &objekt: kruhoveObjekty) {
    switch (objekt.typ) {
      case ASTEROID: {
        kresliAsteroid(surface, objekt);
      } break;

      case LOD: {
        kresliLod(surface, objekt);
      } break;

      case VEC_PUSKA:
      case VEC_BOMBA:
      case VEC_LASER:
      case VEC_URYCHLOVAC:
      case VEC_STIT:
      case VEC_LEKARNICKA: {
        kresliVec(surface, objekt);
      } break;

      default: {
        kresliKruh(surface, objekt.koniec.x, objekt.koniec.y, objekt.polomer, 1, 0, 0, 1);
      } break;
    }
  }
}
