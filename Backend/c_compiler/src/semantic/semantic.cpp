#include "semantic.h"
#include <cstdio>

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::error(const std::string& msg, int line) {
    errors.push_back("Semantic Error (line " + std::to_string(line) + "): " + msg);
}
bool SemanticAnalyzer::typeCompatible(const std::string& a, const std::string& b) {
    if (a == b) return true;
    bool numA = (a=="int"||a=="float"||a=="double"||a=="char");
    bool numB = (b=="int"||b=="float"||b=="double"||b=="char");
    return numA && numB; // implicit numeric conversion allowed
}

bool SemanticAnalyzer::analyze(ASTNode* program) {
    for (ASTNode* n = program->left; n; n = n->next) visitDecl(n);
    return errors.empty();
}

void SemanticAnalyzer::visitDecl(ASTNode* n) {
    if (n->type == NODE_FUNC_DEF) { visitFunc(n); return; }
    if (n->type == NODE_VAR_DECL || n->type == NODE_ARRAY_DECL) {
        Symbol s;
        s.name = n->strval;
        s.type = n->left->label;
        s.kind = (n->type == NODE_ARRAY_DECL) ? SYM_ARRAY : SYM_VAR;
        s.line = n->line;
        if (n->type == NODE_ARRAY_DECL) s.arraySize = n->intval;
        if (!symTab.insert(s)) error("redeclaration of '" + s.name + "'", n->line);
        if (n->right) {
            std::string exprType = visitExpr(n->right);
            if (!exprType.empty() && !typeCompatible(s.type, exprType))
                error("type mismatch in initialization of '" + s.name + "'", n->line);
        }
    }
}

void SemanticAnalyzer::visitFunc(ASTNode* n) {
    Symbol fs;
    fs.name = n->strval;
    fs.type = n->left->label;
    fs.kind = SYM_FUNC;
    fs.line = n->line;
    for (ASTNode* p = n->children[0]; p; p = p->next)
        fs.paramTypes.push_back(p->left->label);
    if (!symTab.insert(fs)) error("redeclaration of function '" + fs.name + "'", n->line);

    currentFuncReturnType = fs.type;
    symTab.enterScope();
    for (ASTNode* p = n->children[0]; p; p = p->next) {
        Symbol ps; ps.name = p->strval; ps.type = p->left->label;
        ps.kind = SYM_PARAM; ps.line = n->line;
        symTab.insert(ps);
    }
    visitStmtList(n->right->left); // body block's stmt_list
    symTab.exitScope();
}

void SemanticAnalyzer::visitStmtList(ASTNode* n) {
    for (ASTNode* s = n; s; s = s->next) visitStmt(s);
}

void SemanticAnalyzer::visitStmt(ASTNode* n) {
    if (!n) return;
    switch (n->type) {
        case NODE_VAR_DECL: case NODE_ARRAY_DECL: visitDecl(n); break;
        case NODE_BLOCK:
            symTab.enterScope(); visitStmtList(n->left); symTab.exitScope(); break;
        case NODE_IF:
            visitExpr(n->left); visitStmt(n->right);
            if (n->children[0]) visitStmt(n->children[0]); break;
        case NODE_WHILE:
            visitExpr(n->left); loopDepth++; visitStmt(n->right); loopDepth--; break;
        case NODE_FOR:
            symTab.enterScope();
            visitStmt(n->left); visitExpr(n->right); visitExpr(n->children[0]);
            loopDepth++; visitStmt(n->children[1]); loopDepth--;
            symTab.exitScope(); break;
        case NODE_RETURN:
            if (n->left) {
                std::string t = visitExpr(n->left);
                if (!t.empty() && !typeCompatible(currentFuncReturnType, t))
                    error("return type mismatch", n->line);
            } else if (currentFuncReturnType != "void") {
                error("missing return value", n->line);
            }
            break;
        case NODE_BREAK: case NODE_CONTINUE:
            if (loopDepth == 0) error("'" + n->label + "' used outside a loop", n->line);
            break;
        default: visitExpr(n); break; // expression statement
    }
}

std::string SemanticAnalyzer::visitExpr(ASTNode* n) {
    if (!n) return "";
    switch (n->type) {
        case NODE_INT_LIT: return "int";
        case NODE_FLOAT_LIT: return "float";
        case NODE_STRING_LIT: return "char*";
        case NODE_CHAR_LIT: return "char";
        case NODE_ID: {
            Symbol* s = symTab.lookup(n->strval);
            if (!s) { error("undeclared identifier '" + n->strval + "'", n->line); return ""; }
            return s->type;
        }
        case NODE_ASSIGN: {
            Symbol* s = symTab.lookup(n->strval);
            if (!s) { error("undeclared identifier '" + n->strval + "'", n->line); return ""; }
            std::string rt = visitExpr(n->right);
            if (!rt.empty() && !typeCompatible(s->type, rt))
                error("type mismatch in assignment to '" + n->strval + "'", n->line);
            return s->type;
        }
        case NODE_BINOP: {
            std::string lt = visitExpr(n->left), rt = visitExpr(n->right);
            if (!lt.empty() && !rt.empty() && !typeCompatible(lt, rt))
                error("type mismatch in binary op '" + n->label + "'", n->line);
            return lt.empty() ? rt : lt;
        }
        case NODE_UNARYOP: return visitExpr(n->left);
        case NODE_CALL: {
            Symbol* s = symTab.lookup(n->strval);
            if (!s || s->kind != SYM_FUNC) { error("undeclared function '" + n->strval + "'", n->line); return ""; }
            size_t argCount = 0;
            for (ASTNode* a = n->left; a; a = a->next) { visitExpr(a); argCount++; }
            if (argCount != s->paramTypes.size())
                error("argument count mismatch calling '" + n->strval + "'", n->line);
            return s->type;
        }
        case NODE_ARRAY_ACCESS: {
            Symbol* s = symTab.lookup(n->strval);
            if (!s) { error("undeclared array '" + n->strval + "'", n->line); return ""; }
            visitExpr(n->left);
            return s->type;
        }
        default: return "";
    }
}