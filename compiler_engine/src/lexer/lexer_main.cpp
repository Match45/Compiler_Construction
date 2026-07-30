#include <cstdio>
#include <iostream>
#include <fstream>
#include "parser.tab.hpp"
#include "../error/error_handler.h"

extern int yylex();
extern char* yytext;
extern int yylineno;
extern FILE* yyin;
YYSTYPE yylval;

static const char* tokenName(int tok) {
    switch (tok) {
        case IDENTIFIER: return "IDENTIFIER";
        case INT_LIT:    return "INT_LIT";
        case FLOAT_LIT:  return "FLOAT_LIT";
        case STRING_LIT: return "STRING_LIT";
        case INT_KW:     return "INT_KW";
        case FLOAT_KW:   return "FLOAT_KW";
        case IF_KW:      return "IF_KW";
        case ELSE_KW:    return "ELSE_KW";
        case WHILE_KW:   return "WHILE_KW";
        case PRINTF_KW:  return "PRINTF_KW";
        case PLUS:       return "PLUS";
        case MINUS:      return "MINUS";
        case MUL:        return "MUL";
        case DIV:        return "DIV";
        case MOD:        return "MOD";
        case LT:         return "LT";
        case GT:         return "GT";
        case LE:         return "LE";
        case GE:         return "GE";
        case EQ:         return "EQ";
        case NE:         return "NE";
        case AND:        return "AND";
        case OR:         return "OR";
        case NOT:        return "NOT";
        case ASSIGN:     return "ASSIGN";
        case LBRACE:     return "LBRACE";
        case RBRACE:     return "RBRACE";
        case LPAREN:     return "LPAREN";
        case RPAREN:     return "RPAREN";
        case SEMI:       return "SEMI";
        case COMMA:      return "COMMA";
        default:         return "UNKNOWN";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./lexical_main <source_file.c>\n";
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) { std::cerr << "Cannot open file: " << argv[1] << "\n"; return 1; }

    std::ofstream outFile("output/tokens.txt");
    auto writeHeader = [&](std::ostream& os) {
        os << "LINE  TOKEN            LEXEME\n";
        os << "----  ---------------  ------\n";
    };
    writeHeader(std::cout);
    if (outFile) writeHeader(outFile);

    int tok;
    char buf[256];
    while ((tok = yylex()) != 0) {
        snprintf(buf, sizeof(buf), "%-6d%-17s%s", yylineno, tokenName(tok), yytext);
        std::cout << buf << "\n";
        if (outFile) outFile << buf << "\n";
    }

    if (ErrorHandler::hasLexicalErrors()) {
        std::cerr << "\n";
        ErrorHandler::printLexicalErrors(std::cerr);
        if (outFile) { outFile << "\n"; ErrorHandler::printLexicalErrors(outFile); }
    }

    std::cout << "\n[Phase 1 output also written to output/tokens.txt]\n";
    return ErrorHandler::hasLexicalErrors() ? 1 : 0;
}