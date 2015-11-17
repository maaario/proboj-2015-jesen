#include <iostream>
#include <unistd.h>
#include <ctime>

using namespace std;

#include "common.h"
#include "marshal.h"

int main() {
  unsigned seed = time(NULL) * getpid();
  srand(seed);

  while (cin.good()) {
    int f=0;
    cout << 10/f;

    string slovo;
    cin >> slovo;

    usleep((rand() % 1000) * 1000);

    cout << slovo << endl << flush;
  }
}
