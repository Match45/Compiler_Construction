#include <cstdio>
#include <cstdlib>
#include <vector>
#include "ast/ast.h"
#include "symbol_table/symbol_table.h"
#include "semantic/semantic.h"
#include "icg/icg.h"
#include "optimizer/optimizer.h"
#include "codegen/codegen.h"

#ifdef _WIN32
#include <windows.h>
#endif

extern int yyparse();
extern FILE* yyin;
extern ASTNode* root;
extern bool showTokens;

static void printTAC(const std::vector<TAC>& code) {
    for (auto& t : code) {
        if (t.op == "func_begin") printf("func %s:\n", t.arg1.c_str());
        else if (t.op == "func_end") printf("end_func %s\n\n", t.arg1.c_str());
        else if (t.op == "label") printf("%s:\n", t.result.c_str());
        else if (t.op == "goto") printf("  goto %s\n", t.result.c_str());
        else if (t.op == "ifFalse") printf("  ifFalse %s goto %s\n", t.arg1.c_str(), t.result.c_str());
        else if (t.op == "=") printf("  %s = %s\n", t.result.c_str(), t.arg1.c_str());
        else if (t.op == "return") printf("  return %s\n", t.arg1.c_str());
        else if (t.op == "param") printf("  param %s\n", t.arg1.c_str());
        else if (t.op == "call") printf("  %s = call %s\n", t.result.c_str(), t.arg1.c_str());
        else if (t.op == "=[]") printf("  %s = %s[%s]\n", t.result.c_str(), t.arg1.c_str(), t.arg2.c_str());
        else if (t.op == "break" || t.op == "continue") printf("  %s\n", t.op.c_str());
        else if (t.arg2.empty()) printf("  %s = %s %s\n", t.result.c_str(), t.op.c_str(), t.arg1.c_str());
        else printf("  %s = %s %s %s\n", t.result.c_str(), t.arg1.c_str(), t.op.c_str(), t.arg2.c_str());
    }
}

int main(int argc, char** argv) {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // ---- Input source ----
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        yyin = stdin;
    }

    // ---- Phase 1: Lexical Analysis (token dump) ----
    printf("=== Phase 1: Lexical Analysis (Tokens) ===\n");
    showTokens = true;
    int parseResult = yyparse();
    showTokens = false;

    if (parseResult != 0 || root == nullptr) {
        printf("\nParsing failed. Stopping.\n");
        if (yyin != stdin) fclose(yyin);
        return 1;
    }

    // ---- Phase 2: Syntax Analysis ----
    printf("\n=== Phase 2: Syntax Analysis ===\n");
    printf("Parsing successful. No syntax errors.\n");

    // ---- Phase 3: AST Construction ----
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
        if (yyin != stdin) fclose(yyin);
        return 1;
    }
    printf("No semantic errors found.\n\n");
    semAnalyzer.getSymbolTable().printTable();

    // ---- Phase 6: Intermediate Code Generation (TAC) ----
    printf("\n=== Phase 6: Intermediate Code Generation (TAC) ===\n");
    ICG icg;
    std::vector<TAC> tacCode = icg.generate(root);
    printTAC(tacCode);

    // ---- Phase 7: Code Optimization ----
    printf("\n=== Phase 7: Code Optimization ===\n");
    Optimizer opt;
    std::vector<TAC> optimizedCode = opt.optimize(tacCode);
    printf("Optimized TAC (%zu instructions, was %zu):\n\n",
           optimizedCode.size(), tacCode.size());
    printTAC(optimizedCode);

    // ---- Phase 8: Target Code Generation (Pseudo-Assembly) ----
    printf("\n=== Phase 8: Target Code Generation (Pseudo-Assembly) ===\n");
    CodeGen codegen;
    codegen.generate(optimizedCode);
    codegen.print();

    if (yyin != stdin) fclose(yyin);
    return 0;
}