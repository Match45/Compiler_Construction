#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"

void semanticCheck(ASTNode *root);

void traverseAST(ASTNode *node);

void processVariableDeclaration(ASTNode *node);

#endif