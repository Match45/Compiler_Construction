#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"
#include <vector>
#include <string>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    bool analyze(ASTNode* program);
    const std::vector<std::string>& getErrors() const { return errors; }
    SymbolTable& getSymbolTable() { return symTab; }

private:
    SymbolTable symTab;
    std::vector<std::string> errors;
    std::string currentFuncReturnType;
    int loopDepth = 0;

    void visitDecl(ASTNode* n);
    void visitFunc(ASTNode* n);
    void visitStmtList(ASTNode* n);
    void visitStmt(ASTNode* n);
    std::string visitExpr(ASTNode* n);
    void error(const std::string& msg, int line);
    bool typeCompatible(const std::string& a, const std::string& b);
};

#endif