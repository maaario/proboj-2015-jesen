
#ifndef LOGIC_H
#define LOGIC_H

#include <string>

struct SDL_Surface;

void nacitajMedia();
void nacitajAdresar(std::string zaznamovyAdresar);
void vykresluj(SDL_Surface *screen, double now);

#endif
