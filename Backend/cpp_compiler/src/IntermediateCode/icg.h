#ifndef ICG_H
#define ICG_H

#include "ast.h"
#include <string>
#include <vector>

struct Quadruple {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    int line;

    Quadruple(std::string o, std::string a1, std::string a2, std::string r, int l = 1)
        : op(o), arg1(a1), arg2(a2), result(r), line(l) {}
};

class IntermediateCodeGenerator {
private:
    std::vector<Quadruple> code;
    int tempCount;
    int labelCount;

    std::string newTemp();
    std::string newLabel();

    std::string generateExpr(ASTNode* expr);
    void generateNode(ASTNode* node);

public:
    IntermediateCodeGenerator() : tempCount(0), labelCount(0) {}

    const std::vector<Quadruple>& generate(ASTNode* root);
};

#endif // ICG_H
