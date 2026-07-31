#include "ast.h"
#include <cstdio>

ASTNode* makeNode(NodeType t, const std::string& label, int line, ASTNode* l, ASTNode* r) {
    ASTNode* n = new ASTNode();
    n->type = t; n->label = label; n->line = line; n->left = l; n->right = r;
    return n;
}

ASTNode* makeProgram() {
    return makeNode(NODE_PROGRAM, "program");
}

ASTNode* makeFuncDef(const std::string& retType, const std::string& name,
                      std::vector<ASTNode*>* params, ASTNode* body, int line) {
    ASTNode* n = makeNode(NODE_FUNC_DEF, "func_def " + retType + " " + name, line, nullptr, body);
    n->strval = name;
    if (params) { n->children = *params; delete params; }
    return n;
}

ASTNode* makeParam(const std::string& type, const std::string& name) {
    ASTNode* n = makeNode(NODE_PARAM, "param " + type + " " + name);
    n->strval = name;
    return n;
}

ASTNode* makeVarDecl(const std::string& type, const std::string& name,
                      ASTNode* initExpr, int line) {
    ASTNode* n = makeNode(NODE_VAR_DECL, "var_decl " + type, line, initExpr, nullptr);
    n->strval = name;
    return n;
}

ASTNode* makeAssign(const std::string& name, ASTNode* expr, int line) {
    ASTNode* n = makeNode(NODE_ASSIGN, "=", line, nullptr, expr);
    n->strval = name;
    return n;
}

ASTNode* makeIncDec(const std::string& name, bool isIncrement, int line) {
    ASTNode* n = makeNode(NODE_INCDEC, isIncrement ? "++" : "--", line);
    n->strval = name;
    n->boolval = isIncrement;
    return n;
}

ASTNode* makeIf(ASTNode* cond, ASTNode* thenBranch, ASTNode* elseBranch, int line) {
    ASTNode* n = makeNode(NODE_IF, "if", line, cond, thenBranch);
    n->children.push_back(elseBranch);
    return n;
}

ASTNode* makeWhile(ASTNode* cond, ASTNode* body, int line) {
    return makeNode(NODE_WHILE, "while", line, cond, body);
}

ASTNode* makeFor(ASTNode* init, ASTNode* cond, ASTNode* update, ASTNode* body, int line) {
    ASTNode* n = makeNode(NODE_FOR, "for", line, init, cond);
    n->children.push_back(update);
    n->children.push_back(body);
    return n;
}

ASTNode* makeCompound(std::vector<ASTNode*>* statements) {
    ASTNode* n = makeNode(NODE_COMPOUND, "block");
    if (statements) { n->children = *statements; delete statements; }
    return n;
}

ASTNode* makeReturn(ASTNode* expr, int line) {
    return makeNode(NODE_RETURN, "return", line, expr, nullptr);
}

ASTNode* makeBreak(int line) { return makeNode(NODE_BREAK, "break", line); }
ASTNode* makeContinue(int line) { return makeNode(NODE_CONTINUE, "continue", line); }

ASTNode* makeCin(std::vector<std::string>* targets, int line) {
    ASTNode* n = makeNode(NODE_CIN, "cin", line);
    if (targets) {
        ASTNode* head = nullptr;
        ASTNode* tail = nullptr;
        for (auto& name : *targets) {
            ASTNode* id = makeIdNode(name, line);
            if (!head) head = id; else tail->next = id;
            tail = id;
        }
        n->left = head;
        delete targets;
    }
    return n;
}

ASTNode* makeCout(std::vector<ASTNode*>* items, int line) {
    ASTNode* n = makeNode(NODE_COUT, "cout", line);
    if (items) { n->children = *items; delete items; }
    return n;
}

ASTNode* makeEndl() {
    return makeNode(NODE_ID, "endl", 0);
}

ASTNode* makeCall(const std::string& name, std::vector<ASTNode*>* args, int line) {
    ASTNode* n = makeNode(NODE_CALL, "call " + name, line);
    n->strval = name;
    if (args) { n->children = *args; delete args; }
    return n;
}

ASTNode* makeBinOp(const std::string& op, ASTNode* l, ASTNode* r, int line) {
    return makeNode(NODE_BINOP, op, line, l, r);
}

ASTNode* makeUnaryOp(const std::string& op, ASTNode* operand, int line) {
    return makeNode(NODE_UNARYOP, op, line, operand, nullptr);
}

ASTNode* makeIdNode(const std::string& name, int line) {
    ASTNode* n = makeNode(NODE_ID, "id", line);
    n->strval = name;
    return n;
}

ASTNode* makeNumberNode(double val, int line) {
    ASTNode* n = makeNode(NODE_NUMBER_LIT, "number_lit", line);
    n->numval = val;
    return n;
}

ASTNode* makeStringNode(const std::string& val, int line) {
    ASTNode* n = makeNode(NODE_STRING_LIT, "string_lit", line);
    n->strval = val;
    return n;
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