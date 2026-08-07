#ifndef TOKENS_H
#define TOKENS_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct TokenInfo {
    string type;
    string lexeme;
    int line;
};

extern vector<TokenInfo> tokenList;

void addToken(const string& type, const string& lexeme, int line);
void printTokenTable();

#endif
