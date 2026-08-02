#include <cstdio>
#include <cstdlib>
#include "ast/ast.h"
#include "symbol_table/symbol_table.h"
#include "semantic/semantic.h"
#include "icg/icg.h"

// from parser (bison generated)
extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;

int main(int argc, char** argv) {

    // ---- Input source: file argument dile file theke, na hoile stdin theke ----
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin; // c_runner.js e stdin diye input dewa hoy
    }

    // ---- Phase 1 & 2: Lexical + Syntax Analysis (flex+bison combined call) ----
    printf("=== Phase 1-2: Lexical + Syntax Analysis ===\n");
    int parseResult = yyparse();
    if (parseResult != 0 || root == nullptr) {
        printf("Parsing failed. Stopping.\n");
        return 1;
    }
    printf("Parsing successful.\n\n");

    // ---- Phase 3: AST Construction (already built during parsing, just print) ----
    printf("=== Phase 3: Abstract Syntax Tree ===\n");
    printAST(root);
    printf("\n");

    // ---- Phase 4 & 5: Symbol Table + Semantic Analysis ----
    printf("=== Phase 4-5: Symbol Table + Semantic Analysis ===\n");
    SemanticAnalyzer semAnalyzer;
    bool ok = semAnalyzer.analyze(root);

    if (!ok) {
        printf("Semantic errors found:\n");
        for (const auto& err : semAnalyzer.getErrors())
            printf("  %s\n", err.c_str());
        printf("\nStopping before code generation due to semantic errors.\n");
        return 1;
    }
    printf("No semantic errors found.\n");
    semAnalyzer.getSymbolTable().printTable();
    printf("\n");

    // ---- Phase 6: Intermediate Code Generation (TAC) ----
    printf("=== Phase 6: Intermediate Code Generation (TAC) ===\n");
    ICG icg;
    icg.generate(root);
    icg.print();

    if (yyin != stdin) fclose(yyin);
    return 0;
}