#include "symbol_table.h"

SymbolTable symbolTable;

bool SymbolTable::insert(string name, string type)
{
    if(table.find(name) != table.end())
    {
        return false;
    }

    table[name] = type;

    return true;
}

bool SymbolTable::exists(string name)
{
    return table.find(name) != table.end();
}

string SymbolTable::getType(string name)
{
    if(exists(name))
        return table[name];

    return "";
}

void SymbolTable::print()
{
    cout << "\n          SYMBOL TABLE           \n";

    for(auto item : table)
    {
        cout << item.first
             << " : "
             << item.second
             << endl;
    }
}