#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <map>
#include <string>

using namespace std;

class SymbolTable
{
private:
    map<string, string> table;

public:

    bool insert(string name, string type);

    bool exists(string name);

    string getType(string name);

    void print();
};

extern SymbolTable symbolTable;

#endif