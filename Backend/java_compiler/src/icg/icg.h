#ifndef ICG_H
#define ICG_H

#include <string>
#include <vector>
#include <iostream>
#include "../ast/ast.h"

using namespace std;

struct TACInstruction {
    string op;
    string arg1;
    string arg2;
    string result;
    string comment;
};

extern vector<TACInstruction> tacInstructions;

void resetICG();
string generateTAC(ASTNode* node);
void printTAC();

#endif
