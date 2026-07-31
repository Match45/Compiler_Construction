#include "icg.h"
#include <cstdio>

std::string ICG::newTemp()  { return "t" + std::to_string(tempCount++); }
std::string ICG::newLabel() { return "L" + std::to_string(labelCount++); }
void ICG::emit(const std::string& op, const std::string& a1, const std::string& a2, const std::string& res) {
    code.push_back({op, a1, a2, res});
}

std::vector<TAC> ICG::generate(ASTNode* program) {
    for (ASTNode* n = program->left; n; n = n->next)
        if (n->type == NODE_FUNC_DEF) genFunc(n);
    return code;
}

void ICG::genFunc(ASTNode* n) {
    emit("func_begin", n->strval, "", "");
    genStmtList(n->right->left);
    emit("func_end", n->strval, "", "");
}

void ICG::genStmtList(ASTNode* n) { for (ASTNode* s = n; s; s = s->next) genStmt(s); }

void ICG::genStmt(ASTNode* n) {
    if (!n) return;
    switch (n->type) {
        case NODE_VAR_DECL:
            if (n->right) { std::string v = genExpr(n->right); emit("=", v, "", n->strval); }
            break;
        case NODE_BLOCK: genStmtList(n->left); break;
        case NODE_IF: {
            std::string cond = genExpr(n->left);
            std::string Lelse = newLabel(), Lend = newLabel();
            emit("ifFalse", cond, "", Lelse);
            genStmt(n->right);
            emit("goto", "", "", Lend);
            emit("label", "", "", Lelse);
            if (n->children[0]) genStmt(n->children[0]);
            emit("label", "", "", Lend);
            break;
        }
        case NODE_WHILE: {
            std::string Lstart = newLabel(), Lend = newLabel();
            emit("label", "", "", Lstart);
            std::string cond = genExpr(n->left);
            emit("ifFalse", cond, "", Lend);
            genStmt(n->right);
            emit("goto", "", "", Lstart);
            emit("label", "", "", Lend);
            break;
        }
        case NODE_FOR: {
            genStmt(n->left); // init
            std::string Lstart = newLabel(), Lend = newLabel();
            emit("label", "", "", Lstart);
            std::string cond = genExpr(n->right);
            emit("ifFalse", cond, "", Lend);
            genStmt(n->children[1]);   // body
            genExpr(n->children[0]);   // step
            emit("goto", "", "", Lstart);
            emit("label", "", "", Lend);
            break;
        }
        case NODE_RETURN: {
            std::string v = n->left ? genExpr(n->left) : "";
            emit("return", v, "", "");
            break;
        }
        case NODE_BREAK: emit("break", "", "", ""); break;
        case NODE_CONTINUE: emit("continue", "", "", ""); break;
        default: genExpr(n); break;
    }
}

std::string ICG::genExpr(ASTNode* n) {
    if (!n) return "";
    switch (n->type) {
        case NODE_INT_LIT: return std::to_string(n->intval);
        case NODE_FLOAT_LIT: return std::to_string(n->floatval);
        case NODE_STRING_LIT: case NODE_CHAR_LIT: return n->strval;
        case NODE_ID: return n->strval;
        case NODE_ASSIGN: {
            std::string v = genExpr(n->right);
            emit("=", v, "", n->strval);
            return n->strval;
        }
        case NODE_BINOP: {
            std::string l = genExpr(n->left), r = genExpr(n->right), t = newTemp();
            emit(n->label, l, r, t);
            return t;
        }
        case NODE_UNARYOP: {
            std::string v = genExpr(n->left), t = newTemp();
            emit(n->label, v, "", t);
            return t;
        }
        case NODE_CALL: {
            for (ASTNode* a = n->left; a; a = a->next) emit("param", genExpr(a), "", "");
            std::string t = newTemp();
            emit("call", n->strval, "", t);
            return t;
        }
        case NODE_ARRAY_ACCESS: {
            std::string idx = genExpr(n->left), t = newTemp();
            emit("=[]", n->strval, idx, t);
            return t;
        }
        default: return "";
    }
}

void ICG::print() const {
    for (auto& t : code) {
        if (t.op == "label") printf("%s:\n", t.result.c_str());
        else if (t.op == "goto") printf("  goto %s\n", t.result.c_str());
        else if (t.op == "ifFalse") printf("  ifFalse %s goto %s\n", t.arg1.c_str(), t.result.c_str());
        else if (t.op == "=") printf("  %s = %s\n", t.result.c_str(), t.arg1.c_str());
        else if (t.op == "return") printf("  return %s\n", t.arg1.c_str());
        else if (t.arg2.empty()) printf("  %s = %s %s\n", t.result.c_str(), t.op.c_str(), t.arg1.c_str());
        else printf("  %s = %s %s %s\n", t.result.c_str(), t.arg1.c_str(), t.op.c_str(), t.arg2.c_str());
    }
}