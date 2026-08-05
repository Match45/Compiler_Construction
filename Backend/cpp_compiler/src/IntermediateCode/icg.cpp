#include "icg.h"

std::string IntermediateCodeGenerator::newTemp() {
    return "t" + std::to_string(tempCount++);
}

std::string IntermediateCodeGenerator::newLabel() {
    return "L" + std::to_string(labelCount++);
}

const std::vector<Quadruple>& IntermediateCodeGenerator::generate(ASTNode* root) {
    code.clear();
    tempCount = 0;
    labelCount = 0;
    if (root) {
        generateNode(root);
    }
    return code;
}

std::string IntermediateCodeGenerator::generateExpr(ASTNode* expr) {
    if (!expr) return "";

    switch (expr->type) {
        case NODE_LITERAL: {
            LiteralNode* lit = static_cast<LiteralNode*>(expr);
            return lit->value;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* id = static_cast<IdentifierNode*>(expr);
            return id->name;
        }
        case NODE_BINARY_EXPR: {
            BinaryExprNode* bin = static_cast<BinaryExprNode*>(expr);
            std::string l = generateExpr(bin->left);
            std::string r = generateExpr(bin->right);
            std::string t = newTemp();
            code.push_back(Quadruple(bin->op, l, r, t, bin->line));
            return t;
        }
        case NODE_UNARY_EXPR: {
            UnaryExprNode* un = static_cast<UnaryExprNode*>(expr);
            std::string val = generateExpr(un->expr);
            std::string t = newTemp();
            code.push_back(Quadruple(un->op, val, "", t, un->line));
            return t;
        }
        case NODE_FUNC_CALL: {
            FuncCallNode* call = static_cast<FuncCallNode*>(expr);
            for (auto arg : call->args) {
                std::string a = generateExpr(arg);
                code.push_back(Quadruple("PARAM", a, "", "", call->line));
            }
            std::string t = newTemp();
            code.push_back(Quadruple("CALL", call->name, std::to_string(call->args.size()), t, call->line));
            return t;
        }
        default:
            return "";
    }
}

void IntermediateCodeGenerator::generateNode(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* prog = static_cast<ProgramNode*>(node);
            for (auto stmt : prog->statements) {
                generateNode(stmt);
            }
            break;
        }
        case NODE_BLOCK: {
            BlockNode* blk = static_cast<BlockNode*>(node);
            for (auto stmt : blk->statements) {
                generateNode(stmt);
            }
            break;
        }
        case NODE_VAR_DECL: {
            VarDeclNode* decl = static_cast<VarDeclNode*>(node);
            code.push_back(Quadruple("DECL", decl->varType, "", decl->name, decl->line));
            if (decl->initExpr) {
                std::string val = generateExpr(decl->initExpr);
                code.push_back(Quadruple("=", val, "", decl->name, decl->line));
            }
            break;
        }
        case NODE_ASSIGN: {
            AssignNode* ass = static_cast<AssignNode*>(node);
            std::string val = generateExpr(ass->expr);
            code.push_back(Quadruple("=", val, "", ass->name, ass->line));
            break;
        }
        case NODE_IF: {
            IfStmtNode* ifNode = static_cast<IfStmtNode*>(node);
            std::string condVal = generateExpr(ifNode->condition);
            std::string elseLabel = newLabel();
            std::string endLabel = newLabel();

            code.push_back(Quadruple("IF_FALSE", condVal, "", ifNode->elseBlock ? elseLabel : endLabel, ifNode->line));
            generateNode(ifNode->thenBlock);

            if (ifNode->elseBlock) {
                code.push_back(Quadruple("GOTO", "", "", endLabel, ifNode->line));
                code.push_back(Quadruple("LABEL", "", "", elseLabel, ifNode->line));
                generateNode(ifNode->elseBlock);
            }
            code.push_back(Quadruple("LABEL", "", "", endLabel, ifNode->line));
            break;
        }
        case NODE_WHILE: {
            WhileStmtNode* whileNode = static_cast<WhileStmtNode*>(node);
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();

            code.push_back(Quadruple("LABEL", "", "", startLabel, whileNode->line));
            std::string condVal = generateExpr(whileNode->condition);
            code.push_back(Quadruple("IF_FALSE", condVal, "", endLabel, whileNode->line));
            generateNode(whileNode->body);
            code.push_back(Quadruple("GOTO", "", "", startLabel, whileNode->line));
            code.push_back(Quadruple("LABEL", "", "", endLabel, whileNode->line));
            break;
        }
        case NODE_FOR: {
            ForStmtNode* forNode = static_cast<ForStmtNode*>(node);
            if (forNode->init) generateNode(forNode->init);

            std::string startLabel = newLabel();
            std::string endLabel = newLabel();

            code.push_back(Quadruple("LABEL", "", "", startLabel, forNode->line));
            if (forNode->condition) {
                std::string condVal = generateExpr(forNode->condition);
                code.push_back(Quadruple("IF_FALSE", condVal, "", endLabel, forNode->line));
            }
            generateNode(forNode->body);
            if (forNode->update) generateNode(forNode->update);
            code.push_back(Quadruple("GOTO", "", "", startLabel, forNode->line));
            code.push_back(Quadruple("LABEL", "", "", endLabel, forNode->line));
            break;
        }
        case NODE_FUNC_DEF: {
            FunctionDefNode* func = static_cast<FunctionDefNode*>(node);
            code.push_back(Quadruple("FUNC_BEGIN", func->returnType, "", func->name, func->line));
            for (const auto& param : func->params) {
                code.push_back(Quadruple("PARAM_DECL", param.type, "", param.name, func->line));
            }
            if (func->body) generateNode(func->body);
            code.push_back(Quadruple("FUNC_END", "", "", func->name, func->line));
            break;
        }
        case NODE_FUNC_CALL: {
            FuncCallNode* call = static_cast<FuncCallNode*>(node);
            for (auto arg : call->args) {
                std::string a = generateExpr(arg);
                code.push_back(Quadruple("PARAM", a, "", "", call->line));
            }
            code.push_back(Quadruple("CALL", call->name, std::to_string(call->args.size()), "", call->line));
            break;
        }
        case NODE_RETURN: {
            ReturnNode* ret = static_cast<ReturnNode*>(node);
            std::string val = ret->expr ? generateExpr(ret->expr) : "";
            code.push_back(Quadruple("RETURN", val, "", "", ret->line));
            break;
        }
        case NODE_COUT: {
            CoutNode* coutNode = static_cast<CoutNode*>(node);
            for (const auto& item : coutNode->items) {
                if (item.isEndl) {
                    code.push_back(Quadruple("COUT_ENDL", "", "", "", coutNode->line));
                } else if (item.expr) {
                    std::string val = generateExpr(item.expr);
                    code.push_back(Quadruple("COUT", val, "", "", coutNode->line));
                }
            }
            break;
        }
        case NODE_CIN: {
            CinNode* cinNode = static_cast<CinNode*>(node);
            for (const auto& name : cinNode->varNames) {
                code.push_back(Quadruple("CIN", "", "", name, cinNode->line));
            }
            break;
        }
        default:
            break;
    }
}
