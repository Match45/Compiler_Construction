#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "../icg/icg.h"
#include <vector>
#include <string>

class Optimizer {
public:
    std::vector<TAC> optimize(const std::vector<TAC>& code);

private:
    std::vector<TAC> constantFolding(const std::vector<TAC>& code);
    std::vector<TAC> removeUnusedLabels(const std::vector<TAC>& code);
    std::vector<TAC> removeDeadCodeAfterJump(const std::vector<TAC>& code);
    bool isNumber(const std::string& s);
};

#endif