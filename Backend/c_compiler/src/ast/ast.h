#ifndef AST_H
#define AST_H
#include <string>
#include <vector>

enum NodeType {
    NODE_PROGRAM, NODE_FUNC_DEF, NODE_PARAM, NODE_VAR_DECL, NODE_ARRAY_DECL,
    NODE_BLOCK, NODE_IF, NODE_WHILE, NODE_FOR, NODE_RETURN,
    NODE_BREAK, NODE_CONTINUE, NODE_ASSIGN, NODE_BINOP, NODE_UNARYOP,
    NODE_ID, NODE_INT_LIT, NODE_FLOAT_LIT, NODE_STRING_LIT, NODE_CHAR_LIT,
    NODE_CALL, NODE_ARRAY_ACCESS, NODE_TYPE
};

struct ASTNode {
    NodeType type;
    std::string label;      // op / name / type name
    std::string strval;
    int intval = 0;
    float floatval = 0;
    int line = 0;

    ASTNode* left = nullptr;
    ASTNode* right = nullptr;
    ASTNode* next = nullptr; // sibling chain (for lists)
    std::vector<ASTNode*> children;
};

ASTNode* makeNode(NodeType t, const std::string& label, int line, ASTNode* l, ASTNode* r);
ASTNode* makeTypeNode(const std::string& typeName);
ASTNode* makeVarDecl(ASTNode* type, const char* name, ASTNode* initExpr, int line);
ASTNode* makeArrayDecl(ASTNode* type, const char* name, int size, int line);
ASTNode* makeFuncDef(ASTNode* type, const char* name, ASTNode* params, ASTNode* body, int line);
ASTNode* makeParam(ASTNode* type, const char* name);
ASTNode* makeIf(ASTNode* cond, ASTNode* thenS, ASTNode* elseS, int line);
ASTNode* makeWhile(ASTNode* cond, ASTNode* body, int line);
ASTNode* makeFor(ASTNode* init, ASTNode* cond, ASTNode* step, ASTNode* body, int line);
ASTNode* makeReturn(ASTNode* expr, int line);
ASTNode* makeAssign(const char* name, ASTNode* expr, int line);
ASTNode* makeBinOp(const std::string& op, ASTNode* l, ASTNode* r, int line);
ASTNode* makeUnaryOp(const std::string& op, ASTNode* operand, int line);
ASTNode* makeIdNode(const char* name, int line);
ASTNode* makeIntNode(int val, int line);
ASTNode* makeFloatNode(float val, int line);
ASTNode* makeStringNode(const char* val, int line);
ASTNode* makeCharNode(const char* val, int line);
ASTNode* makeCall(const char* name, ASTNode* args, int line);
ASTNode* makeArrayAccess(const char* name, ASTNode* idx, int line);
ASTNode* appendSibling(ASTNode* list, ASTNode* item);
void printAST(ASTNode* node, int depth = 0);

#endif