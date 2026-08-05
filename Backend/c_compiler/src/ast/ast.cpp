#include "ast.h"
#include <cstdio>
#include <vector>
#include <string>

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

// ---------- Tree-style AST printer ----------

static void printASTHelper(ASTNode* node, std::string prefix, bool isLast) {
    if (!node) return;

    printf("%s", prefix.c_str());
    printf("%s", isLast ? "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 " : "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 ");
    // "└── " and "├── " in UTF-8 bytes (safe for Windows console with UTF-8 codepage)

    printf("%s", node->label.c_str());
    if (!node->strval.empty()) printf(" [%s]", node->strval.c_str());
    if (node->type == NODE_INT_LIT) printf(" = %d", node->intval);
    if (node->type == NODE_FLOAT_LIT) printf(" = %g", node->floatval);
    printf(" (line %d)\n", node->line);

    std::string newPrefix = prefix + (isLast ? "    " : "\xE2\x94\x82   ");
    // "    " if last, "│   " otherwise

    std::vector<ASTNode*> kids;
    if (node->left) kids.push_back(node->left);
    if (node->right) kids.push_back(node->right);
    for (auto c : node->children) if (c) kids.push_back(c);

    for (size_t i = 0; i < kids.size(); i++) {
        bool lastChild = (i == kids.size() - 1);
        printASTHelper(kids[i], newPrefix, lastChild);
    }
}

void printAST(ASTNode* node, int depth) {
    (void)depth; // no longer used, kept for header compatibility

    std::vector<ASTNode*> siblings;
    for (ASTNode* n = node; n; n = n->next) siblings.push_back(n);

    for (size_t i = 0; i < siblings.size(); i++) {
        bool lastSibling = (i == siblings.size() - 1);
        printASTHelper(siblings[i], "", lastSibling);
    }
}