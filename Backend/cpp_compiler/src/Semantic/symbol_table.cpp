#include "symbol_table.h"

SymbolTable::SymbolTable() {
    currentScope = 0;
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::enterScope() {
    currentScope++;
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::exitScope() {
    if (currentScope > 0) {
        scopes.pop_back();
        currentScope--;
    }
}

bool SymbolTable::insertVariable(const std::string& name, const std::string& type, int line) {
    if (scopes[currentScope].find(name) != scopes[currentScope].end()) {
        return false;
    }
    Symbol sym(name, SYMBOL_VAR, type, currentScope, line);
    scopes[currentScope][name] = sym;
    return true;
}

bool SymbolTable::insertFunction(const std::string& name, const std::string& returnType, const std::vector<std::string>& paramTypes, int line) {
    int targetScope = 0;
    if (scopes[targetScope].find(name) != scopes[targetScope].end()) {
        return false;
    }
    Symbol sym(name, SYMBOL_FUNC, returnType, targetScope, line);
    sym.paramTypes = paramTypes;
    scopes[targetScope][name] = sym;
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    for (int i = currentScope; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            return &(it->second);
        }
    }
    return nullptr;
}

Symbol* SymbolTable::lookupCurrentScope(const std::string& name) {
    auto it = scopes[currentScope].find(name);
    if (it != scopes[currentScope].end()) {
        return &(it->second);
    }
    return nullptr;
}
