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
    void clear();
    const map<string, string>& getTable() const { return table; }
};

extern SymbolTable symbolTable;

#endif
