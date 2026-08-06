#include <iostream>
#include "semantic.h"
#include "../symbol_table/symbol_table.h"

using namespace std;

void processVariableDeclaration(ASTNode *node)
{
    if(node->children.size() < 2)
        return;

    string type=node->children[0]->value;
    string name=node->children[1]->value;

    cout<<"Variable Declaration Found : "
        <<type<<" "
        <<name<<endl;

    if(!symbolTable.insert(name,type))
    {
        cout<<"Semantic Error : Variable "
            <<name
            <<" already declared."
            <<endl;

        return;
    }

    if(node->children.size()==3)
    {
        string exprType=getExpressionType(node->children[2]);

        if(type!=exprType)
        {
            cout<<"Semantic Error : Cannot assign "
                <<exprType
                <<" to "
                <<type
                <<endl;
        }
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

string getExpressionType(ASTNode *node)
{
    if(node == nullptr)
        return "";

    if(node->type == "Integer")
        return "int";

    if(node->type == "Float")
        return "float";

    if(node->type == "Boolean")
        return "boolean";

    if(node->type == "Char")
        return "char";

    if(node->type == "String")
        return "String";

    if(node->type == "Identifier")
    {
        return symbolTable.getType(node->value);
    }

    if(node->type=="+" ||
       node->type=="-" ||
       node->type=="*" ||
       node->type=="/" ||
       node->type=="%")
    {
        string left=getExpressionType(node->children[0]);
        string right=getExpressionType(node->children[1]);

        if(left=="float" || right=="float")
            return "float";

        return "int";
    }

    return "";
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