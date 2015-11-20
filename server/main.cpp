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

int volne_id = 0;
int volny_hrac = 0;

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

  // nacitaj mapu
  string mapovySubor(argv[2]);
  Mapa mapa;
  if (!nacitajMapu(mapa,mapovySubor)) {
    return 1;
  }
  stringstream popisMapy;
  uloz(popisMapy,mapa);

  // vytvor zaznamovy adresar
  string zaznamovyAdresar(argv[1]);
  if (!jeAdresar(zaznamovyAdresar)) {
    if (mkdir(zaznamovyAdresar.c_str(), 0777)) {
      fprintf(stderr, "mkdir: %s: %s\n", zaznamovyAdresar.c_str(), strerror(errno));
      return 1;
    }
  }

  // nacitame klientov
  vector<string> klientskeAdresare;
  for (int i = 3; i < argc; ++i) {
    klientskeAdresare.push_back(string(argv[i]));
  }
  for (int i = 0; i < (int)klientskeAdresare.size(); ++i) {
    klienti.push_back(Klient(itos(i), klientskeAdresare[i], zaznamovyAdresar));
  }
  random_shuffle(klienti.begin(),klienti.end());

  // zostroj pociatocny stav
  Stav stav;
  if (!pociatocnyStav(mapa,stav,klienti.size()) ) {
    return 1;
  }
  stringstream popisStavu;
  uloz(popisStavu,stav);

  // spustime klientov
  log("spustam klientov");
  for (Klient &klient: klienti) {
    klient.spusti();

    //ak mi klient dosial zomrel, tak...
    klient.posli(popisMapy.str().c_str());
    klient.posli(popisStavu.str().c_str());
  }

  const long long fps = 25LL;
  const long long frame_time = 1000LL / fps;
  ofstream observationstream((zaznamovyAdresar+"/observation").c_str());
  zapniObservation(&observationstream,frame_time);

  //
  // hlavny cyklus
  //

  const long long ups = 100LL;   // updates per second
  const long long delta_time = 1000LL / ups;
  long long last_time = gettime();

  vector<Prikaz> akcie;

  while (stav.zivychHracov() > 1) {

    for (Klient &klient: klienti) {
      int kolkaty = akcie.size();
      akcie.push_back(Prikaz());
      
      if (klient.nebezi()) {
        klient.restartuj(stav.cas);
        /*
        if (!klient.nebezi()) {
          klient.posli(popisMapy.str().c_str());
          klient.posli(popisStavu.str().c_str());
        }
        */
        continue;
      }
      
      string odpoved = klient.citaj();
      if (odpoved == "") { //klient pocita/vypisuje (nie nicnerobi, to by bolo "\n")
        continue;
      }
      stringstream buf;
      buf << odpoved;
      nacitaj(buf,akcie[kolkaty]);

      //log("klient \"%s\" napisal: %s", klient.getLabel().c_str(),odpoved.c_str());
      Hrac temp= stav.hraci[0];
      stav.hraci[0]= stav.hraci[kolkaty];
      stav.hraci[kolkaty]= temp;
      popisStavu.str("");
      uloz(popisStavu,stav);
      klient.posli(popisStavu.str().c_str());
      stav.hraci[kolkaty]=stav.hraci[0];
      stav.hraci[0]=temp;
    }

    odsimuluj(mapa,stav,akcie);

    //cleanup
    akcie.clear();

    // pockame, aby sme dodrzovali zelane UPS
    long long current_time = gettime();
    if (current_time < last_time + delta_time) {
      long long remaining_ms = (last_time + delta_time) - current_time;
      usleep(remaining_ms * 1000LL);
    }
    last_time = gettime();
  }

  // end step
  //
  log("ukoncujeme klientov");
  zabiKlientov();
  
  observationstream.close();

  return 0;
}

