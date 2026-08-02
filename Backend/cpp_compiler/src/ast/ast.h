#ifndef AST_H
#define AST_H
#include <string>
#include <vector>

enum NodeType {
    NODE_PROGRAM, NODE_FUNC_DEF, NODE_PARAM, NODE_VAR_DECL,
    NODE_COMPOUND, NODE_IF, NODE_WHILE, NODE_FOR,
    NODE_RETURN, NODE_BREAK, NODE_CONTINUE,
    NODE_ASSIGN, NODE_INCDEC, NODE_COUT, NODE_CIN,
    NODE_CALL, NODE_BINOP, NODE_UNARYOP,
    NODE_ID, NODE_NUMBER_LIT, NODE_STRING_LIT
};

struct ASTNode {
    NodeType type;
    std::string label;      // op / keyword / node kind label, used for printing
    std::string strval;     // identifier name / string literal text / cin target name
    double numval = 0;      // NUMBER value
    bool boolval = false;   // true = increment, false = decrement (NODE_INCDEC only)
    int line = 0;

    ASTNode* left = nullptr;
    ASTNode* right = nullptr;
    ASTNode* next = nullptr;             // sibling chain (used to link cin targets)
    std::vector<ASTNode*> children;      // generic list: params, args, statements,
                                          // cout items, and the func_def's param list
};

// generic constructor, same shape as the C AST's makeNode
ASTNode* makeNode(NodeType t, const std::string& label, int line = 0,
                   ASTNode* l = nullptr, ASTNode* r = nullptr);

ASTNode* makeProgram();
ASTNode* makeFuncDef(const std::string& retType, const std::string& name,
                      std::vector<ASTNode*>* params, ASTNode* body, int line);
ASTNode* makeParam(const std::string& type, const std::string& name);
ASTNode* makeVarDecl(const std::string& type, const std::string& name,
                      ASTNode* initExpr, int line);
ASTNode* makeAssign(const std::string& name, ASTNode* expr, int line);
ASTNode* makeIncDec(const std::string& name, bool isIncrement, int line);
ASTNode* makeIf(ASTNode* cond, ASTNode* thenBranch, ASTNode* elseBranch, int line);
ASTNode* makeWhile(ASTNode* cond, ASTNode* body, int line);
ASTNode* makeFor(ASTNode* init, ASTNode* cond, ASTNode* update, ASTNode* body, int line);
ASTNode* makeCompound(std::vector<ASTNode*>* statements);
ASTNode* makeReturn(ASTNode* expr, int line);
ASTNode* makeBreak(int line);
ASTNode* makeContinue(int line);
ASTNode* makeCin(std::vector<std::string>* targets, int line);
ASTNode* makeCout(std::vector<ASTNode*>* items, int line);
ASTNode* makeEndl();
ASTNode* makeCall(const std::string& name, std::vector<ASTNode*>* args, int line);
ASTNode* makeBinOp(const std::string& op, ASTNode* l, ASTNode* r, int line);
ASTNode* makeUnaryOp(const std::string& op, ASTNode* operand, int line);
ASTNode* makeIdNode(const std::string& name, int line);
ASTNode* makeNumberNode(double val, int line);
ASTNode* makeStringNode(const std::string& val, int line);

void printAST(ASTNode* node, int depth = 0);

#endif