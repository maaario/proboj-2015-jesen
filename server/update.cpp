
#include <ostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

#include "common.h"
#include "update.h"

static ostream* g_observation;
void zapniObservation(ostream* observation) { g_observation = observation; }
