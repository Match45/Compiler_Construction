#include <iostream>
#include "semantic.h"

using namespace std;

void semanticCheck(ASTNode *root)
{
    cout << endl;
    cout << "========== SEMANTIC ANALYSIS ==========" << endl;

    if(root == nullptr)
    {
        cout << "AST is empty." << endl;
        return;
    }

    cout << "Semantic Analysis Started..." << endl;

    // Next Part থেকে এখানে checking হবে

    cout << "Semantic Analysis Completed." << endl;
}