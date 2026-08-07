#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include "../icg/icg.h"

using namespace std;

extern vector<TACInstruction> optimizedTac;

void optimizeTAC(const vector<TACInstruction>& original);
void printOptimizedTAC();

#endif
