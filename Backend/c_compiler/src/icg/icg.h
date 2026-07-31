#ifndef ICG_H
#define ICG_H
#include "../ast/ast.h"
#include <vector>
#include <string>

struct TAC {
    std::string op, arg1, arg2, result;
};

class ICG {
public:
    std::vector<TAC> generate(ASTNode* program);
    void print() const;

private:
    std::vector<TAC> code;
    int tempCount = 0, labelCount = 0;
    std::string newTemp();
    std::string newLabel();
    void emit(const std::string& op, const std::string& a1, const std::string& a2, const std::string& res);

    void genFunc(ASTNode* n);
    void genStmtList(ASTNode* n);
    void genStmt(ASTNode* n);
    std::string genExpr(ASTNode* n);
};

#endif