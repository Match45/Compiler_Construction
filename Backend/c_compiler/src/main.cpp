#include <cstdio>
#include <cstdlib>
#include "ast/ast.h"
#include "symbol_table/symbol_table.h"
#include "semantic/semantic.h"
#include "icg/icg.h"

#ifdef _WIN32
#include <windows.h>
#endif

extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;
extern bool showTokens;

int main(int argc, char** argv) {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin;
    }

    // ... বাকি code অপরিবর্তিত থাকবে

    // ---- Phase 1: Lexical Analysis (token dump) ----
    printf("=== Phase 1: Lexical Analysis (Tokens) ===\n");
    showTokens = true;

    // ---- Phase 2-3: Syntax Analysis + AST Construction ----
    int parseResult = yyparse();
    showTokens = false;

    if (parseResult != 0 || root == nullptr) {
        printf("\nParsing failed. Stopping.\n");
        return 1;
    }
    printf("\n=== Phase 2: Syntax Analysis ===\n");
    printf("Parsing successful. No syntax errors.\n");

    printf("\n=== Phase 3: Abstract Syntax Tree ===\n");
    printAST(root);

    // ---- Phase 4-5: Symbol Table + Semantic Analysis ----
    printf("\n=== Phase 4-5: Symbol Table + Semantic Analysis ===\n");
    SemanticAnalyzer semAnalyzer;
    bool ok = semAnalyzer.analyze(root);

    if (!ok) {
        printf("Semantic errors found:\n");
        for (const auto& err : semAnalyzer.getErrors())
            printf("  %s\n", err.c_str());
        printf("\nStopping before code generation due to semantic errors.\n");
        return 1;
    }
    printf("No semantic errors found.\n\n");
    semAnalyzer.getSymbolTable().printTable();

    // ---- Phase 6: Intermediate Code Generation (TAC) ----
    printf("\n=== Phase 6: Intermediate Code Generation (TAC) ===\n");
    ICG icg;
    icg.generate(root);
    icg.print();

    if (yyin != stdin) fclose(yyin);
    return 0;
}