
#ifndef UTIL_H
#define UTIL_H

#include <string>

#define INF 1023456789ll

void inicializujSignaly(void (*cleanupFunkcia)());

#ifdef NELOGUJ
#define log(...) (0)
#else
void logheader();
#include <cstdio>
#define log(...) (logheader(), fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"))
#endif

bool jeAdresar(std::string);
bool jeSubor(std::string);

long long gettime();
std::string itos(int i);

#endif
