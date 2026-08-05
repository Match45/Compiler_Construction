#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

enum SymbolKind {
    SYMBOL_VAR,
    SYMBOL_FUNC
};

struct Symbol {
    std::string name;
    SymbolKind kind;
    std::string type;
    int scopeLevel;
    int lineDeclared;
    
    std::vector<std::string> paramTypes;
    
    Symbol() : name(""), kind(SYMBOL_VAR), type(""), scopeLevel(0), lineDeclared(0) {}
    Symbol(std::string n, SymbolKind k, std::string t, int scope, int line)
        : name(n), kind(k), type(t), scopeLevel(scope), lineDeclared(line) {}
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int currentScope;

public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool insertVariable(const std::string& name, const std::string& type, int line);
    bool insertFunction(const std::string& name, const std::string& returnType, const std::vector<std::string>& paramTypes, int line);

    Symbol* lookup(const std::string& name);
    Symbol* lookupCurrentScope(const std::string& name);

    int getCurrentScopeLevel() const { return currentScope; }
};

#endif // SYMBOL_TABLE_H
