#include "ast.h"
#include <cstdio>

ASTNode* makeNode(NodeType t, const std::string& label, int line, ASTNode* l, ASTNode* r) {
    ASTNode* n = new ASTNode();
    n->type = t; n->label = label; n->line = line; n->left = l; n->right = r;
    return n;
}
ASTNode* makeTypeNode(const std::string& typeName) {
    return makeNode(NODE_TYPE, typeName, 0, nullptr, nullptr);
}
ASTNode* makeVarDecl(ASTNode* type, const char* name, ASTNode* initExpr, int line) {
    ASTNode* n = makeNode(NODE_VAR_DECL, "var_decl", line, type, initExpr);
    n->strval = name; return n;
}
ASTNode* makeArrayDecl(ASTNode* type, const char* name, int size, int line) {
    ASTNode* n = makeNode(NODE_ARRAY_DECL, "array_decl", line, type, nullptr);
    n->strval = name; n->intval = size; return n;
}
ASTNode* makeFuncDef(ASTNode* type, const char* name, ASTNode* params, ASTNode* body, int line) {
    ASTNode* n = makeNode(NODE_FUNC_DEF, "func_def", line, type, body);
    n->strval = name; n->children.push_back(params); return n;
}
ASTNode* makeParam(ASTNode* type, const char* name) {
    ASTNode* n = makeNode(NODE_PARAM, "param", 0, type, nullptr);
    n->strval = name; return n;
}
ASTNode* makeIf(ASTNode* cond, ASTNode* thenS, ASTNode* elseS, int line) {
    ASTNode* n = makeNode(NODE_IF, "if", line, cond, thenS);
    n->children.push_back(elseS); return n;
}
ASTNode* makeWhile(ASTNode* cond, ASTNode* body, int line) {
    return makeNode(NODE_WHILE, "while", line, cond, body);
}
ASTNode* makeFor(ASTNode* init, ASTNode* cond, ASTNode* step, ASTNode* body, int line) {
    ASTNode* n = makeNode(NODE_FOR, "for", line, init, cond);
    n->children.push_back(step); n->children.push_back(body); return n;
}
ASTNode* makeReturn(ASTNode* expr, int line) {
    return makeNode(NODE_RETURN, "return", line, expr, nullptr);
}
ASTNode* makeAssign(const char* name, ASTNode* expr, int line) {
    ASTNode* n = makeNode(NODE_ASSIGN, "=", line, nullptr, expr);
    n->strval = name; return n;
}
ASTNode* makeBinOp(const std::string& op, ASTNode* l, ASTNode* r, int line) {
    return makeNode(NODE_BINOP, op, line, l, r);
}
ASTNode* makeUnaryOp(const std::string& op, ASTNode* operand, int line) {
    return makeNode(NODE_UNARYOP, op, line, operand, nullptr);
}
ASTNode* makeIdNode(const char* name, int line) {
    ASTNode* n = makeNode(NODE_ID, "id", line, nullptr, nullptr);
    n->strval = name; return n;
}
ASTNode* makeIntNode(int val, int line) {
    ASTNode* n = makeNode(NODE_INT_LIT, "int_lit", line, nullptr, nullptr);
    n->intval = val; return n;
}
ASTNode* makeFloatNode(float val, int line) {
    ASTNode* n = makeNode(NODE_FLOAT_LIT, "float_lit", line, nullptr, nullptr);
    n->floatval = val; return n;
}
ASTNode* makeStringNode(const char* val, int line) {
    ASTNode* n = makeNode(NODE_STRING_LIT, "string_lit", line, nullptr, nullptr);
    n->strval = val; return n;
}
ASTNode* makeCharNode(const char* val, int line) {
    ASTNode* n = makeNode(NODE_CHAR_LIT, "char_lit", line, nullptr, nullptr);
    n->strval = val; return n;
}
ASTNode* makeCall(const char* name, ASTNode* args, int line) {
    ASTNode* n = makeNode(NODE_CALL, "call", line, args, nullptr);
    n->strval = name; return n;
}
ASTNode* makeArrayAccess(const char* name, ASTNode* idx, int line) {
    ASTNode* n = makeNode(NODE_ARRAY_ACCESS, "array_access", line, idx, nullptr);
    n->strval = name; return n;
}
ASTNode* appendSibling(ASTNode* list, ASTNode* item) {
    if (!list) return item;
    ASTNode* cur = list;
    while (cur->next) cur = cur->next;
    cur->next = item;
    return list;
}
void printAST(ASTNode* node, int depth) {
    for (ASTNode* n = node; n; n = n->next) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("%s (line %d)\n", n->label.c_str(), n->line);
        if (n->left) printAST(n->left, depth + 1);
        if (n->right) printAST(n->right, depth + 1);
        for (auto c : n->children) if (c) printAST(c, depth + 1);
    }
}