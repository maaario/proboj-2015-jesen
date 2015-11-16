#include <iostream>
#include <unistd.h>
#include <ctime>

using namespace std;

int main() {
  unsigned seed = time(NULL) * getpid();
  srand(seed);

  while (cin.good()) {
    string slovo;
    cin >> slovo;

    usleep((rand() % 1000) * 1000);

    cout << slovo << endl << flush;
  }
}
