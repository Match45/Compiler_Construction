#include <iostream>
#include "semantic.h"
#include "../symbol_table/symbol_table.h"

using namespace std;

void processVariableDeclaration(ASTNode *node)
{
    if(node->children.size() < 2)
        return;

    string type = node->children[0]->value;
    string name = node->children[1]->value;

    cout << "Variable Declaration Found : "
         << type
         << " "
         << name
         << endl;

    if(!symbolTable.insert(name, type))
    {
        cout << "Semantic Error : Variable "
             << name
             << " already declared."
             << endl;
    }
}

void processAssignment(ASTNode *node)
{
    if(node->children.size() < 2)
        return;

    string name = node->children[0]->value;

    cout << "Assignment Found : "
         << name
         << endl;

    if(!symbolTable.exists(name))
    {
        cout << "Semantic Error : Variable "
             << name
             << " not declared."
             << endl;
    }
}

void traverseAST(ASTNode *node)
{
    if(node == nullptr)
        return;

    cout << "Visiting Node : " << node->type;

    if(node->value != "")
        cout << " (" << node->value << ")";

    cout << endl;

    if(node->type == "VariableDeclaration")
    {
        processVariableDeclaration(node);
    }

    if(node->type == "Assignment")
    {
        processAssignment(node);
    }

    for(ASTNode *child : node->children)
    {
        traverseAST(child);
    }
}

void semanticCheck(ASTNode *root)
{
    cout << endl;
    cout << "            SEMANTIC ANALYSIS          " << endl;

    if(root == nullptr)
    {
        cout << "AST is empty." << endl;
        return;
    }
    symbolTable.clear();

    cout << "Semantic Analysis Started..." << endl;
    cout << endl;

    traverseAST(root);

    cout << endl;
    cout << "Semantic Analysis Completed." << endl;
   
}