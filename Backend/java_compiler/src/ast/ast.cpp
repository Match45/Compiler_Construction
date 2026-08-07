#include "ast.h"

ASTNode::ASTNode(string t, string v)
{
    type = t;
    value = v;
}

void ASTNode::addChild(ASTNode* node)
{
    children.push_back(node);
}

void printAST(ASTNode* node, int level)
{
    if (node == nullptr)
        return;

    for (int i = 0; i < level; i++)
        cout << "   ";

    cout << node->type;

    if (node->value != "")
        cout << " : " << node->value;

    cout << endl;

    for (auto child : node->children)
        printAST(child, level + 1);
}
