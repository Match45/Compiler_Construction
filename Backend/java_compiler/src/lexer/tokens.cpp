#include "tokens.h"
#include <iomanip>

vector<TokenInfo> tokenList;

void addToken(const string& type, const string& lexeme, int line) {
    tokenList.push_back({type, lexeme, line});
}

void printTokenTable() {
    cout << "\n...............\n";
    cout << "          PHASE 1: LEXICAL ANALYSIS (TOKENS)           \n";
    cout << "...............\n";
    cout << left << setw(8) << "Line" << setw(22) << "Token Type" << "Lexeme" << endl;
    cout << "---------------\n";
    for (const auto& tok : tokenList) {
        cout << left << setw(8) << tok.line << setw(22) << tok.type << tok.lexeme << endl;
    }
    cout << "...............\n";
}
