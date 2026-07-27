#include <iostream>
#include <fstream>
#include <cstdio>
#include "ast.h"
#include "../error/error_handler.h"

extern int yyparse();
extern FILE* yyin;
extern ASTPtr programRoot;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./syntax_main <source_file.c>\n";
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) { std::cerr << "Cannot open file: " << argv[1] << "\n"; return 1; }

    int result = yyparse();

    if (ErrorHandler::hasLexicalErrors()) {
        std::cerr << "\n"; ErrorHandler::printLexicalErrors(std::cerr);
    }

    if (result != 0 || ErrorHandler::hasSyntaxErrors()) {
        ErrorHandler::printSyntaxErrors(std::cerr);
        std::cerr << "\nPhase 2 halted: syntax errors found.\n";
        return 1;
    }

    std::cout << "Parsing completed successfully. AST built.\n\n";
    printAST(programRoot);

    std::ofstream outFile("output/ast.txt");
    if (outFile) {
        printAST(programRoot, 0, outFile);
        std::cout << "\n[AST also written to output/ast.txt]\n";
    }

    return 0;
}