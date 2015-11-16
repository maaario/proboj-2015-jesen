#ifndef KLIENT_H
#define KLIENT_H

#include <vector>
#include <string>

#include "proces.h"

class Klient {
  public:
    std::string label;
    Proces proces;
    std::string precitane;
  public:
    Klient(std::string _label, std::string adresar, std::string logAdresar);
    std::string getLabel();
    void spusti();
    std::string citaj();
    void posli(std::string data);
    void zabi();
};

#endif
