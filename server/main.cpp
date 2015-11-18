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
#include <algorithm>
using namespace std;

#include "util.h"
#include "common.h"
#include "update.h"
#include "klient.h"
#include "marshal.h"
#include "mapa.h"

vector<Klient> klienti;

// tato trapna funkcia existuje len kvoli inicializujSignaly()
void zabiKlientov() {
  for (Klient &klient: klienti) {
    log("zabijam klienta %s",klient.getLabel().c_str());
    klient.zabi();
  }
}

int main(int argc, char *argv[]) {
  unsigned int seed = time(NULL) * getpid();
  srand(seed);

  if (argc < 3) {
    fprintf(stderr, "usage: %s <zaznamovy-adresar> <mapa> {<adresare-klientov> ...}\n", argv[0]);
    return 1;
  }

  log("startujem server, seed = %u", seed);
  inicializujSignaly(zabiKlientov);

  //
  // zaznamovy adresar
  //

  string zaznamovyAdresar(argv[1]);
  string mapovySubor(argv[2]);

  //
  // nacitame klientov
  //
  vector<string> klientskeAdresare;
  for (int i = 3; i < argc; ++i) {
    klientskeAdresare.push_back(string(argv[i]));
  }

  //nacitaj mapu
  Mapa mapa;
  nacitajMapu(mapa,mapovySubor);
  stringstream popisMapy;
  uloz(popisMapy,mapa);

  if (!jeAdresar(zaznamovyAdresar)) {
    if (mkdir(zaznamovyAdresar.c_str(), 0777)) {
      fprintf(stderr, "mkdir: %s: %s\n", zaznamovyAdresar.c_str(), strerror(errno));
      return 1;
    }
  }
  for (int i = 0; i < klientskeAdresare.size(); ++i) {
    klienti.push_back(Klient(itos(i), klientskeAdresare[i], zaznamovyAdresar));
  }
  
  //eliminovat "baklazan je najlepsi, tak ho fokusneme"
  random_shuffle(klienti.begin(),klienti.end());

  //zostroj pociatocny stav
  Stav stav;
  pociatocnyStav(mapa,stav,klienti.size());
  stringstream popisStavu;
  uloz(popisStavu,stav);

  //
  // spustime klientov
  //
  log("spustam klientov");
  for (Klient &klient: klienti) {
    klient.spusti();

    //posli pociatocny stav
    klient.posli(popisMapy.str().c_str());
    klient.posli(popisStavu.str().c_str());
  }

  //
  // hlavny cyklus
  //

  const long long ups = 100LL;   // updates per second
  const long long delta_time = 1000LL / ups;
  long long last_time = gettime();
  long long tick = 0;

  vector<Prikaz> akcie;

  while (true) {
    // pockame, aby sme dodrzovali zelane UPS
    long long current_time = gettime();
    if (current_time < last_time + delta_time) {
      long long remaining_ms = (last_time + delta_time) - current_time;
      usleep(remaining_ms * 1000LL);
    }
    tick += 1;

    popisStavu.str("");
    uloz(popisStavu,stav);

    for (Klient &klient: klienti) {
      int kolkaty = akcie.size();
      akcie.push_back(Prikaz());
      
      if (klient.nebezi()) {
        klient.restartuj(current_time);
        if (!klient.nebezi()) {
          klient.posli(popisMapy.str().c_str());
          klient.posli(popisStavu.str().c_str());
        }
        continue;
      }
      
      string odpoved = klient.citaj();
      if (odpoved == "") { //klient pocita/vypisuje (nie nicnerobi, to by bolo "\n")
        continue;
      }
      stringstream buf;
      buf << odpoved;
      nacitaj(buf,akcie[kolkaty]);

      log("klient \"%s\" napisal: %s", klient.getLabel().c_str(), odpoved.c_str());
      klient.posli(popisStavu.str().c_str());
    }

    current_time = gettime();
    odsimuluj(mapa,stav,akcie,current_time - last_time);
    last_time = current_time;

    //cleanup
    akcie.clear();
  }

  //
  // zabijeme klientov
  //
  log("ukoncujeme klientov");
  zabiKlientov();

  return 0;
}

