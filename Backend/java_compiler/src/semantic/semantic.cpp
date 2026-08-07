#include <iostream>
#include "semantic.h"
#include "../symbol_table/symbol_table.h"

using namespace std;

vector<string> semanticLogs;

static void logSemantic(const string& msg) {
    cout << msg << endl;
    semanticLogs.push_back(msg);
}

void processVariableDeclaration(ASTNode *node)
{
    if (node->children.size() < 2) return;

    string type = node->children[0]->value;
    string name = node->children[1]->value;

    logSemantic("Variable Declaration Found : " + type + " " + name);

    if (!symbolTable.insert(name, type))
    {
        logSemantic("Semantic Error : Variable '" + name + "' already declared.");
        return;
    }

    if (node->children.size() == 3)
    {
        string exprType = getExpressionType(node->children[2]);
        if (!exprType.empty() && type != exprType)
        {
            if (type == "double" && exprType == "float") {
                // allowed promotion
            } else if (type == "float" && exprType == "int") {
                // allowed promotion
            } else {
                logSemantic("Semantic Error : Cannot assign " + exprType + " to " + type + " '" + name + "'");
            }
        }
    }
}

void processAssignment(ASTNode *node)
{
    if (node->children.size() < 2) return;

    string name = node->children[0]->value;

    logSemantic("Assignment Found : " + name);

    if (!symbolTable.exists(name))
    {
        logSemantic("Semantic Error : Variable '" + name + "' not declared.");
    }
    else
    {
        string varType = symbolTable.getType(name);
        string exprType = getExpressionType(node->children[1]);
        if (!exprType.empty() && varType != exprType)
        {
            if (varType == "double" && exprType == "float") {
                // allowed
            } else if (varType == "float" && exprType == "int") {
                // allowed
            } else {
                logSemantic("Semantic Error : Cannot assign " + exprType + " to " + varType + " '" + name + "'");
            }
        }
    }
}

void traverseAST(ASTNode *node)
{
    if (node == nullptr) return;

    if (node->type == "VariableDeclaration") {
        processVariableDeclaration(node);
    }

    if (node->type == "Assignment") {
        processAssignment(node);
    }

    for (ASTNode *child : node->children) {
        traverseAST(child);
    }
}

string getExpressionType(ASTNode *node)
{
    if (node == nullptr) return "";

    if (node->type == "Integer") return "int";
    if (node->type == "Float") return "float";
    if (node->type == "Boolean") return "boolean";
    if (node->type == "Char") return "char";
    if (node->type == "String") return "String";

    if (node->type == "Identifier") {
        return symbolTable.getType(node->value);
    }

    if (node->type == "+" || node->type == "-" || node->type == "*" || node->type == "/" || node->type == "%") {
        if (node->children.size() < 2) return "";
        string left = getExpressionType(node->children[0]);
        string right = getExpressionType(node->children[1]);

        if (left == "String" || right == "String") return "String";
        if (left == "float" || right == "float") return "float";
        if (left == "double" || right == "double") return "double";
        return "int";
    }

    return "";
}

void semanticCheck(ASTNode *root)
{
    semanticLogs.clear();
    cout << "\n...............\n";
    cout << "     PHASE 3: SEMANTIC ANALYSIS & SYMBOL TABLE         \n";
    cout << "...............\n";

    if (root == nullptr) {
        logSemantic("AST is empty.");
        return;
    }
    symbolTable.clear();

    logSemantic("Semantic Analysis Started...");
    traverseAST(root);
    logSemantic("Semantic Analysis Completed.");
    cout << "...............\n";
}
