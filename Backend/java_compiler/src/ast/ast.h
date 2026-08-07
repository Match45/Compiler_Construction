#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class ASTNode
{
public:
    string type;
    string value;
    vector<ASTNode*> children;

    ASTNode(string t, string v = "");
    void addChild(ASTNode* node);
};

void printAST(ASTNode* node, int level = 0);

#endif
