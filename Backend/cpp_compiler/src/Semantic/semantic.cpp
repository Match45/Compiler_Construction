#include "semantic.h"
#include <iostream>

bool SemanticAnalyzer::analyze(ASTNode* root) {
    errors.clear();
    if (!root) return true;
    analyzeNode(root);
    return errors.empty();
}

void SemanticAnalyzer::analyzeNode(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            analyzeProgram(static_cast<ProgramNode*>(node));
            break;
        case NODE_BLOCK:
            analyzeBlock(static_cast<BlockNode*>(node));
            break;
        case NODE_VAR_DECL:
            analyzeVarDecl(static_cast<VarDeclNode*>(node));
            break;
        case NODE_ASSIGN:
            analyzeAssign(static_cast<AssignNode*>(node));
            break;
        case NODE_IF:
            analyzeIf(static_cast<IfStmtNode*>(node));
            break;
        case NODE_WHILE:
            analyzeWhile(static_cast<WhileStmtNode*>(node));
            break;
        case NODE_FOR:
            analyzeFor(static_cast<ForStmtNode*>(node));
            break;
        case NODE_FUNC_DEF:
            analyzeFunctionDef(static_cast<FunctionDefNode*>(node));
            break;
        case NODE_FUNC_CALL:
            analyzeFuncCall(static_cast<FuncCallNode*>(node));
            break;
        case NODE_RETURN:
            analyzeReturn(static_cast<ReturnNode*>(node));
            break;
        case NODE_COUT:
            analyzeCout(static_cast<CoutNode*>(node));
            break;
        case NODE_CIN:
            analyzeCin(static_cast<CinNode*>(node));
            break;
        default:
            break;
    }
}

void SemanticAnalyzer::analyzeProgram(ProgramNode* node) {
    for (auto stmt : node->statements) {
        analyzeNode(stmt);
    }
}

void SemanticAnalyzer::analyzeBlock(BlockNode* node) {
    symbolTable.enterScope();
    for (auto stmt : node->statements) {
        analyzeNode(stmt);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::analyzeVarDecl(VarDeclNode* node) {
    if (!symbolTable.insertVariable(node->name, node->varType, node->line)) {
        errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                         ": Redeclaration of variable '" + node->name + "'");
    }
    if (node->initExpr) {
        analyzeExpr(node->initExpr);
    }
}

void SemanticAnalyzer::analyzeAssign(AssignNode* node) {
    Symbol* sym = symbolTable.lookup(node->name);
    if (!sym) {
        errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                         ": Undeclared variable '" + node->name + "'");
    }
    if (node->expr) {
        analyzeExpr(node->expr);
    }
}

void SemanticAnalyzer::analyzeIf(IfStmtNode* node) {
    if (node->condition) analyzeExpr(node->condition);
    if (node->thenBlock) analyzeNode(node->thenBlock);
    if (node->elseBlock) analyzeNode(node->elseBlock);
}

void SemanticAnalyzer::analyzeWhile(WhileStmtNode* node) {
    if (node->condition) analyzeExpr(node->condition);
    if (node->body) analyzeNode(node->body);
}

void SemanticAnalyzer::analyzeFor(ForStmtNode* node) {
    symbolTable.enterScope();
    if (node->init) analyzeNode(node->init);
    if (node->condition) analyzeExpr(node->condition);
    if (node->update) analyzeNode(node->update);
    if (node->body) analyzeNode(node->body);
    symbolTable.exitScope();
}

void SemanticAnalyzer::analyzeFunctionDef(FunctionDefNode* node) {
    std::vector<std::string> paramTypes;
    for (const auto& param : node->params) {
        paramTypes.push_back(param.type);
    }

    if (!symbolTable.insertFunction(node->name, node->returnType, paramTypes, node->line)) {
        errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                         ": Redeclaration of function '" + node->name + "'");
    }

    symbolTable.enterScope();
    for (const auto& param : node->params) {
        if (!symbolTable.insertVariable(param.name, param.type, node->line)) {
            errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                             ": Duplicate parameter '" + param.name + "' in function '" + node->name + "'");
        }
    }

    if (node->body) {
        analyzeNode(node->body);
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::analyzeFuncCall(FuncCallNode* node) {
    Symbol* sym = symbolTable.lookup(node->name);
    if (!sym || sym->kind != SYMBOL_FUNC) {
        errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                         ": Call to undeclared function '" + node->name + "'");
    } else {
        if (node->args.size() != sym->paramTypes.size()) {
            errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                             ": Function '" + node->name + "' expects " +
                             std::to_string(sym->paramTypes.size()) + " arguments, but got " +
                             std::to_string(node->args.size()));
        }
    }

    for (auto arg : node->args) {
        analyzeExpr(arg);
    }
}

void SemanticAnalyzer::analyzeReturn(ReturnNode* node) {
    if (node->expr) {
        analyzeExpr(node->expr);
    }
}

void SemanticAnalyzer::analyzeCout(CoutNode* node) {
    for (const auto& item : node->items) {
        if (item.expr) {
            analyzeExpr(item.expr);
        }
    }
}

void SemanticAnalyzer::analyzeCin(CinNode* node) {
    for (const auto& varName : node->varNames) {
        Symbol* sym = symbolTable.lookup(varName);
        if (!sym) {
            errors.push_back("Semantic Error on Line " + std::to_string(node->line) +
                             ": Undeclared variable '" + varName + "' in cin statement");
        }
    }
}

std::string SemanticAnalyzer::analyzeExpr(ASTNode* expr) {
    if (!expr) return "void";

    switch (expr->type) {
        case NODE_LITERAL: {
            LiteralNode* lit = static_cast<LiteralNode*>(expr);
            return lit->valType;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* id = static_cast<IdentifierNode*>(expr);
            Symbol* sym = symbolTable.lookup(id->name);
            if (!sym) {
                errors.push_back("Semantic Error on Line " + std::to_string(expr->line) +
                                 ": Undeclared variable '" + id->name + "'");
                return "unknown";
            }
            return sym->type;
        }
        case NODE_BINARY_EXPR: {
            BinaryExprNode* bin = static_cast<BinaryExprNode*>(expr);
            std::string t1 = analyzeExpr(bin->left);
            std::string t2 = analyzeExpr(bin->right);
            return (t1 == "float" || t2 == "float") ? "float" : "int";
        }
        case NODE_UNARY_EXPR: {
            UnaryExprNode* un = static_cast<UnaryExprNode*>(expr);
            return analyzeExpr(un->expr);
        }
        case NODE_FUNC_CALL: {
            FuncCallNode* call = static_cast<FuncCallNode*>(expr);
            analyzeFuncCall(call);
            Symbol* sym = symbolTable.lookup(call->name);
            return sym ? sym->type : "unknown";
        }
        default:
            analyzeNode(expr);
            return "int";
    }
}
