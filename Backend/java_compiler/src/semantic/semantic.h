#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <vector>
#include <string>
#include "../ast/ast.h"

using namespace std;

extern vector<string> semanticLogs;

void semanticCheck(ASTNode *root);
void traverseAST(ASTNode *node);
void processAssignment(ASTNode *node);
string getExpressionType(ASTNode *node);
void processVariableDeclaration(ASTNode *node);

#endif
