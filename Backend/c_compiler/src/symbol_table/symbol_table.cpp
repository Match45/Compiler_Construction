#include "symbol_table.h"
#include <cstdio>

SymbolTable::SymbolTable() { enterScope(); } // global scope

void SymbolTable::enterScope() {
    scopeStack.push_back({});
}
void SymbolTable::exitScope() {
    if (!scopeStack.empty()) scopeStack.pop_back();
}
bool SymbolTable::insert(const Symbol& sym) {
    auto& top = scopeStack.back();
    if (top.find(sym.name) != top.end()) return false; // redeclaration
    Symbol s = sym;
    s.scopeLevel = currentScope();
    top[sym.name] = s;
    return true;
}
Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return &found->second;
    }
    return nullptr;
}
void SymbolTable::printTable() const {
    int lvl = 0;
    for (auto& scope : scopeStack) {
        printf("--- Scope %d ---\n", lvl++);
        for (auto& [name, sym] : scope)
            printf("  %-10s type=%-6s kind=%d line=%d\n",
                   name.c_str(), sym.type.c_str(), sym.kind, sym.line);
    }
}