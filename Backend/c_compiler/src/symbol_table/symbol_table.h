#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <string>
#include <vector>
#include <unordered_map>

enum SymbolKind { SYM_VAR, SYM_ARRAY, SYM_FUNC, SYM_PARAM };

struct Symbol {
    std::string name;
    std::string type;      // int/float/char/double/void
    SymbolKind kind;
    int scopeLevel;
    int arraySize = 0;
    std::vector<std::string> paramTypes; // for functions
    int line;
};

class SymbolTable {
public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool insert(const Symbol& sym);          // false if redeclared in current scope
    Symbol* lookup(const std::string& name);  // searches outward through scopes
    int currentScope() const { return scopeStack.size() - 1; }
    void printTable() const;

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopeStack;
};

#endif