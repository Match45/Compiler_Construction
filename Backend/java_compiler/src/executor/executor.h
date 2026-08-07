#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include <vector>
#include <iostream>
#include "../ast/ast.h"

using namespace std;

extern vector<string> executionOutput;

void executeAST(ASTNode* root);
void printExecutionOutput();

#endif
