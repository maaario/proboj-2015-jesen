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
#include "mapa.h"


vector<Klient> klienti;


static void zabiKlientov() {
  log("ukoncujem klientov");
  for (unsigned i = 0; i < klienti.size(); i++) {
    klienti[i].zabi();
  }
}


template<class T> void checkOstream(T& s, string filename) {
  if (s.fail()) {
    fprintf(stderr, "neviem zapisovat do %s\n", filename.c_str());
    zabiKlientov();
    exit(1);
  }
}


int main(int argc, char *argv[]) {
  unsigned int seed = time(NULL) * getpid();
  srand(seed);

  if (argc < 4) {
    fprintf(stderr, "usage: %s <zaznamovy-adresar> <mapa> {<adresare-klientov>...}\n", argv[0]);
    fprintf(stderr, "vypnutie timeoutov: export NO_TIMEOUTS=true\n");
    return 1;
  }

  log("startujem server, seed je %u", seed);

  inicializujSignaly(zabiKlientov);

  string zaznamovyAdresar(argv[1]);
  string mapovySubor(argv[2]);
  vector<string> klientskeAdresare;
  for (int i = 3; i < argc; i++) klientskeAdresare.push_back(string(argv[i]));

  //nacitame mapu
  Mapa mapa;
  if (!nacitajMapu(mapa, mapovySubor, klientskeAdresare.size())) return 1;

  if (!jeAdresar(zaznamovyAdresar)) {
    if (mkdir(zaznamovyAdresar.c_str(), 0777)) {
      fprintf(stderr, "mkdir: %s: %s\n", zaznamovyAdresar.c_str(), strerror(errno));
      return 1;
    }
  }

  if (jeSubor(zaznamovyAdresar+"/map")) {
    fprintf(stderr, "subor %s uz existuje\n", (zaznamovyAdresar+"/map").c_str());
    fprintf(stderr, "daj mi novy prazdny zaznamovy adresar, v tomto uz nieco je.\n");
    return 1;
  }
  stringstream mapabuf;
  uloz(mapabuf, mapa);
  ofstream mapastream((zaznamovyAdresar+"/map").c_str());
  mapastream << mapabuf.str();
  mapastream.close();
  checkOstream(mapastream, zaznamovyAdresar+"/map");

  ofstream logstream((zaznamovyAdresar+"/log").c_str());
  checkOstream(logstream, zaznamovyAdresar+"/log");

  ofstream formatstream((zaznamovyAdresar+"/format").c_str());
  checkOstream(formatstream, zaznamovyAdresar+"/format");
  formatstream << FORMAT_VERSION;
  formatstream.close();

  for (unsigned i = 0; i < klientskeAdresare.size(); i++) {
    klienti.push_back(Klient(itos(i), mapabuf.str(),
                             klientskeAdresare[i], zaznamovyAdresar));
    klienti[i].restartuj();
  }

  char *noTimeouts = getenv("NO_TIMEOUTS");
  if (noTimeouts && *noTimeouts) {
    log("NO_TIMEOUTS - davam klientom neobmedzeny cas na odpoved");
    for (unsigned i = 0; i < klienti.size(); i++) klienti[i].vypniTimeout();
  }

  ofstream observationstream((zaznamovyAdresar+"/observation").c_str());
  checkOstream(observationstream, zaznamovyAdresar+"/observation");
  zapniObservation(&observationstream);

  //TODO: tu by asi mal byt hlavny cyklus

  logstream.close();
  observationstream.close();

  zabiKlientov();

  vector<int> vysledky(klienti.size());
  for (unsigned i = 0; i < klienti.size(); i++) {
    vysledky[i] = 0;  //TODO
  }

  ofstream rankstream((zaznamovyAdresar+"/rank").c_str());
  uloz(rankstream, vysledky);
  rankstream.close();
  checkOstream(rankstream, zaznamovyAdresar+"/rank");

  log("skoncili sme");
  return 0;
}

