//SECURITY WARNING
//Chod prec uboha dusa. temne su miesta kam vchadzas
#include <climits>
#include <unistd.h>
using namespace std;

#include "klient.h"
#include "util.h"

#define CAS_KLIENTA_NA_INICIALIZACIU 4000
#define CAS_KLIENTA_NA_ODPOVED 500
#define MAXIMUM_RESTARTOV 3


Klient::Klient(string _label, string cwd, string zaznamovyAdresar) : label(_label), precitane("") {
  vector<string> command;
  command.push_back("./hrac");
  proces.setProperties(command, cwd,
                       zaznamovyAdresar + "/stderr." + label);
}

string Klient::citaj() {
  precitane += proces.nonblockRead();
  if (precitane.back() == '.') {    // vraciame az hotovu odpoved, t.j. uzavretu znakom '.'
    string navrat = precitane;
    navrat.pop_back();              // odstranime bodku
    precitane.clear();              // resetujeme precitane
    return navrat;
  } else {
    return "";
  }
}

void Klient::posli(string data) {
  proces.write(data);
}

void Klient::zabi() {
  proces.zabi();
}
