//SECURITY WARNING
//Chod prec uboha dusa. temne su miesta kam vchadzas
#include <iostream>
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

string Klient::getLabel() {
  return label;
}

void Klient::spusti() {
  proces.restartuj();
}

string Klient::citaj() {
  precitane += proces.nonblockRead();
  if (precitane.size() > 0 && precitane.back() == '\n') {   // vraciame az hotovu odpoved, uzavretu znakom noveho riadku
    string navrat = precitane;
    precitane.clear();                                      // resetujeme precitane
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
