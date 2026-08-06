#ifndef AST_H
#define AST_H
#include <string>
#include <vector>
#include <iostream>
#include <memory>

enum NodeType {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_BINARY_EXPR,
    NODE_UNARY_EXPR,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_RETURN,
    NODE_COUT,
    NODE_CIN,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_BLOCK
};

class ASTNode {
public:
    NodeType type;
    int line;

    ASTNode(NodeType t, int l = 1) : type(t), line(l) {}
    virtual ~ASTNode() {}
};

class ProgramNode : public ASTNode {
public:
    std::vector<ASTNode*> statements;
    ProgramNode(int l = 1) : ASTNode(NODE_PROGRAM, l) {}
    ~ProgramNode() {
        for (auto stmt : statements) delete stmt;
    }
};

class BlockNode : public ASTNode {
public:
    std::vector<ASTNode*> statements;
    BlockNode(int l = 1) : ASTNode(NODE_BLOCK, l) {}
    ~BlockNode() {
        for (auto stmt : statements) delete stmt;
    }
};

class VarDeclNode : public ASTNode {
public:
    std::string varType;
    std::string name;
    ASTNode* initExpr;

    VarDeclNode(std::string vt, std::string n, ASTNode* init = nullptr, int l = 1)
        : ASTNode(NODE_VAR_DECL, l), varType(vt), name(n), initExpr(init) {}
    ~VarDeclNode() {
        if (initExpr) delete initExpr;
    }
};

class AssignNode : public ASTNode {
public:
    std::string name;
    ASTNode* expr;

    AssignNode(std::string n, ASTNode* e, int l = 1)
        : ASTNode(NODE_ASSIGN, l), name(n), expr(e) {}
    ~AssignNode() {
        if (expr) delete expr;
    }
};

class BinaryExprNode : public ASTNode {
public:
    std::string op;
    ASTNode* left;
    ASTNode* right;

    BinaryExprNode(std::string o, ASTNode* l_node, ASTNode* r_node, int l = 1)
        : ASTNode(NODE_BINARY_EXPR, l), op(o), left(l_node), right(r_node) {}
    ~BinaryExprNode() {
        delete left;
        delete right;
    }
};

class UnaryExprNode : public ASTNode {
public:
    std::string op;
    ASTNode* expr;

    UnaryExprNode(std::string o, ASTNode* e, int l = 1)
        : ASTNode(NODE_UNARY_EXPR, l), op(o), expr(e) {}
    ~UnaryExprNode() {
        delete expr;
    }
};

class LiteralNode : public ASTNode {
public:
    std::string valType; // "int", "float", "string"
    std::string value;

    LiteralNode(std::string vt, std::string val, int l = 1)
        : ASTNode(NODE_LITERAL, l), valType(vt), value(val) {}
};

class IdentifierNode : public ASTNode {
public:
    std::string name;

    IdentifierNode(std::string n, int l = 1)
        : ASTNode(NODE_IDENTIFIER, l), name(n) {}
};

class IfStmtNode : public ASTNode {
public:
    ASTNode* condition;
    ASTNode* thenBlock;
    ASTNode* elseBlock;

    IfStmtNode(ASTNode* cond, ASTNode* tb, ASTNode* eb = nullptr, int l = 1)
        : ASTNode(NODE_IF, l), condition(cond), thenBlock(tb), elseBlock(eb) {}
    ~IfStmtNode() {
        delete condition;
        delete thenBlock;
        if (elseBlock) delete elseBlock;
    }
};

class WhileStmtNode : public ASTNode {
public:
    ASTNode* condition;
    ASTNode* body;

    WhileStmtNode(ASTNode* cond, ASTNode* b, int l = 1)
        : ASTNode(NODE_WHILE, l), condition(cond), body(b) {}
    ~WhileStmtNode() {
        delete condition;
        delete body;
    }
};

class ForStmtNode : public ASTNode {
public:
    ASTNode* init;
    ASTNode* condition;
    ASTNode* update;
    ASTNode* body;

    ForStmtNode(ASTNode* i, ASTNode* cond, ASTNode* u, ASTNode* b, int l = 1)
        : ASTNode(NODE_FOR, l), init(i), condition(cond), update(u), body(b) {}
    ~ForStmtNode() {
        if (init) delete init;
        if (condition) delete condition;
        if (update) delete update;
        delete body;
    }
};

class ReturnNode : public ASTNode {
public:
    ASTNode* expr;

    ReturnNode(ASTNode* e = nullptr, int l = 1)
        : ASTNode(NODE_RETURN, l), expr(e) {}
    ~ReturnNode() {
        if (expr) delete expr;
    }
};

struct CoutItem {
    ASTNode* expr;
    bool isEndl;
};

class CoutNode : public ASTNode {
public:
    std::vector<CoutItem> items;

    CoutNode(int l = 1) : ASTNode(NODE_COUT, l) {}
    ~CoutNode() {
        for (auto item : items) {
            if (item.expr) delete item.expr;
        }
    }
};

class CinNode : public ASTNode {
public:
    std::vector<std::string> varNames;

    CinNode(int l = 1) : ASTNode(NODE_CIN, l) {}
};

struct Parameter {
    std::string type;
    std::string name;
};

class FunctionDefNode : public ASTNode {
public:
    std::string returnType;
    std::string name;
    std::vector<Parameter> params;
    ASTNode* body;

    FunctionDefNode(std::string rt, std::string n, std::vector<Parameter> p, ASTNode* b, int l = 1)
        : ASTNode(NODE_FUNC_DEF, l), returnType(rt), name(n), params(p), body(b) {}
    ~FunctionDefNode() {
        delete body;
    }
};

class FuncCallNode : public ASTNode {
public:
    std::string name;
    std::vector<ASTNode*> args;

    FuncCallNode(std::string n, std::vector<ASTNode*> a, int l = 1)
        : ASTNode(NODE_FUNC_CALL, l), name(n), args(a) {}
    ~FuncCallNode() {
        for (auto arg : args) delete arg;
    }
};

#endif