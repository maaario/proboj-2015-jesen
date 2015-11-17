//SECURITY WARNING
//Chod prec uboha dusa. temne su miesta kam vchadzas
#include <iostream>
#include <climits>
#include <unistd.h>
using namespace std;

#include "klient.h"
#include "util.h"

#define MIN_CAS_RESTART 2000ll

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
  poslRestart= -INF;
  proces.restartuj();
}

void Klient::restartuj(long long cas) {
  if (poslRestart== -INF) {
    log("klient \"%s\": BSOD, restartujem...",label.c_str());
    poslRestart= cas;
    return;
  }
  if (cas-poslRestart < MIN_CAS_RESTART) {
    return ;
  }
  log("klient \"%s\": zadavam login a password...",label.c_str());
  poslRestart= -INF;
  precitane.clear();
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

bool Klient::nebezi() {
  bool err = proces.nebezi();
  return err;
}
