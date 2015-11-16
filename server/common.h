
#ifndef COMMON_H
#define COMMON_H

// vseobecne datove struktury

#include <vector>
#include <string>
#include <cstdio>

struct Bod {
  double x, y;

  Bod() {
    this->x = 0;
    this->y = 0;
  }

  Bod(double x, double y) {
    this->x = x;
    this->y = y;
  }

  Bod operator+(const Bod &iny) const {
    return Bod(x + iny.x, y + iny.y);
  }

  Bod operator-(const Bod &iny) const {
    return Bod(x - iny.x, y - iny.y);
  }

  Bod operator*(const double &k) const {
    return Bod(x * k, y * k);
  }
};

struct Spawn {
  Bod pozicia;

  Spawn(double x, double y) {
    pozicia.x = x;
    pozicia.y = y;
  }
};

struct Hrac {
  Bod pozicia, rychlost;
  Bod akceleracia;

  Hrac(double x, double y, double v_x, double v_y) {
    pozicia.x = x;
    pozicia.y = y;
    rychlost.x = v_x;
    rychlost.y = v_y;
    akceleracia.x = 0;
    akceleracia.y = 0;
  }
};

struct Planeta {
  Bod pozicia;
  double polomer;

  Planeta(double x, double y, double polomer) {
    pozicia.x = x;
    pozicia.y = y;
    this->polomer = polomer;
  }
};

struct Hra {
  vector<Spawn> spawny;
  vector<Hrac> hraci;
  vector<Planeta> planety;
};

#endif
