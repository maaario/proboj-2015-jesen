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


}

