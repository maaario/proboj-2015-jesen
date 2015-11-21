#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <string>
#include "SDL.h"
#include "SDL_ttf.h"
using namespace std;

#include "logic.h"

#define FPS 30

#define FARBA_POZADIA   0x101010

int main(int argc, char *argv[]) {
  if (argc != 2 || argv[1][0] == '-') {
    fprintf(stderr, "usage: %s <zaznamovy-adresar>\n", argv[0]);
    return 0;
  }

  srand(time(NULL) * getpid());

  string zaznamovyAdresar(argv[1]);
  Hra hra;
  hra.nacitajSubor(zaznamovyAdresar + "/observation");

  SDL_Init(SDL_INIT_VIDEO);
  atexit(SDL_Quit);

  TTF_Init();
  atexit(TTF_Quit);

  SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 32, SDL_SWSURFACE);

  string title = string("Observer - ") + argv[1];
  SDL_WM_SetCaption(title.c_str(), title.c_str());

  double cas = 0.0;   // na ktorom ticku hry sme, samozrejme az po zaokruhleni
  double rychlost = 1.0;
  bool pauza = false;

  while (true) {
    int pocetTickovNaZaciatku = SDL_GetTicks();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) return 1;

      if (event.type == SDL_KEYDOWN) {
        int key = event.key.keysym.sym;

        switch (key) {
          case SDLK_RETURN: {
            cas = 0;
          } break;

          case SDLK_p: {
            pauza = !pauza;
          } break;

          case SDLK_r: {
            rychlost *= -1;
          } break;

          case SDLK_EQUALS:
          case SDLK_PLUS: {
            rychlost *= 1.25;
          } break;

          case SDLK_MINUS: {
            rychlost *= 0.8;
          } break;
        }
      }
    }

    if (!pauza) cas += rychlost;
    if (cas < 0) cas = 0;
    if (cas >= hra.framy.size()) cas = hra.framy.size() - 1;

    int tick = floor(cas);

    SDL_FillRect(screen, &screen->clip_rect, FARBA_POZADIA);    // vycistime obrazovku

    hra.framy[tick].kresli(screen);                             // nakreslime sucasny stav

    SDL_Flip(screen);                                           // zobrazime, co sme nakreslili, na obrazovku

    int pocetTickovNaKonci = SDL_GetTicks();
    SDL_Delay(1000 / FPS - (pocetTickovNaKonci - pocetTickovNaZaciatku));
  }
}
