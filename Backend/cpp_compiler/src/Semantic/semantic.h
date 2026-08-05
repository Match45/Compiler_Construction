#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"
#include <vector>
#include <string>

class SemanticAnalyzer {
private:
    SymbolTable symbolTable;
    std::vector<std::string> errors;

    void analyzeNode(ASTNode* node);
    void analyzeProgram(ProgramNode* node);
    void analyzeBlock(BlockNode* node);
    void analyzeVarDecl(VarDeclNode* node);
    void analyzeAssign(AssignNode* node);
    void analyzeIf(IfStmtNode* node);
    void analyzeWhile(WhileStmtNode* node);
    void analyzeFor(ForStmtNode* node);
    void analyzeFunctionDef(FunctionDefNode* node);
    void analyzeFuncCall(FuncCallNode* node);
    void analyzeReturn(ReturnNode* node);
    void analyzeCout(CoutNode* node);
    void analyzeCin(CinNode* node);
    std::string analyzeExpr(ASTNode* expr);

public:
    SemanticAnalyzer() {}

    bool analyze(ASTNode* root);
    const std::vector<std::string>& getErrors() const { return errors; }
};

#endif // SEMANTIC_H
