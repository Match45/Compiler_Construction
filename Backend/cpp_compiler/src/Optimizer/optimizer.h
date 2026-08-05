#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "icg.h"
#include <vector>

class CodeOptimizer {
private:
    bool isNumber(const std::string& s, double& outVal);

public:
    CodeOptimizer() {}

    std::vector<Quadruple> optimize(const std::vector<Quadruple>& inputCode);
};

#endif // OPTIMIZER_H
