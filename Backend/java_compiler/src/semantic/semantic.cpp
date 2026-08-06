#include <iostream>
#include "semantic.h"

using namespace std;

void traverseAST(ASTNode *node)
{
    if(node == nullptr)
        return;

    cout << "Visiting Node : " << node->type;

    if(node->value != "")
        cout << " (" << node->value << ")";

    cout << endl;

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

    cout << "Semantic Analysis Started..." << endl;
    cout << endl;

    traverseAST(root);

    cout << endl;
    cout << "Semantic Analysis Completed." << endl;
}