#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

#include "util.h"
#include "common.h"
#include "update.h"
#include "klient.h"
#include "marshal.h"

vector<Klient> klienti;

// tato trapna funkcia existuje len kvoli inicializujSignaly()
void zabiKlientov() {
  for (Klient &klient: klienti) {
    log("zabijam klienta");
    klient.zabi();
  }
}

int main(int argc, char *argv[]) {
  unsigned int seed = time(NULL) * getpid();
  srand(seed);

  if (argc < 3) {
    fprintf(stderr, "usage: %s <zaznamovy-adresar> {<adresare-klientov> ...}\n", argv[0]);
    return 1;
  }

  log("startujem server, seed = %u", seed);
  inicializujSignaly(zabiKlientov);

  //
  // zaznamovy adresar
  //

  string zaznamovyAdresar(argv[1]);

  //
  // nacitame klientov
  //

  vector<string> klientskeAdresare;
  for (int i = 2; i < argc; ++i) {
    klientskeAdresare.push_back(string(argv[i]));

  }

  for (int i = 0; i < klientskeAdresare.size(); ++i) {
    klienti.push_back(Klient(itos(i), klientskeAdresare[i], zaznamovyAdresar));
  }

  //
  // spustime klientov
  //

  log("spustam klientov");
  for (Klient &klient: klienti) {
    klient.spusti();
    klient.posli("hello\n");
  }

  //
  // hlavny cyklus
  //

  const long long ups = 100LL;   // updates per second
  const long long delta_time = 1000LL / ups;
  long long last_time = gettime();
  long long tick = 0;

  while (true) {
    // pockame, aby sme dodrzovali zelane UPS
    long long current_time = gettime();
    if (current_time < last_time + delta_time) {
      long long remaining_ms = (last_time + delta_time) - current_time;
      usleep(remaining_ms * 1000LL);
    }
    last_time = gettime();
    tick += 1;

    for (Klient &klient: klienti) {
      string prikaz = klient.citaj();
      if (prikaz == "") continue;

      log("klient \"%s\" napisal: %s", klient.getLabel().c_str(), prikaz.c_str());
      klient.posli(itos(tick) + "\n");
    }
  }

  //
  // zabijeme klientov
  //

  log("ukoncujeme klientov");
  zabiKlientov();

  return 0;
}

