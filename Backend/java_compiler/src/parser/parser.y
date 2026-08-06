%code requires {
    #include "../ast/ast.h"
    #include "../symbol_table/symbol_table.h"
    #include "../semantic/semantic.h"
    #include "../lexer/tokens.h"
    #include "../icg/icg.h"
    #include "../optimizer/optimizer.h"
    #include "../executor/executor.h"
}

%{
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"
#include "../semantic/semantic.h"
#include "../lexer/tokens.h"
#include "../icg/icg.h"
#include "../optimizer/optimizer.h"
#include "../executor/executor.h"

using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *s);

ASTNode *root = nullptr;
bool syntaxErrorOccurred = false;

static string escapeJSON(const string& s) {
    ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    o << "\\u" << hex << (int)c;
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}

%}

%union{
    int ival;
    float fval;
    char *sval;
    ASTNode *node;
}

%token PUBLIC PRIVATE PROTECTED CLASS STATIC VOID MAIN
%token INT FLOAT DOUBLE CHAR BOOLEAN STRING
%token IF ELSE FOR WHILE DO RETURN
%token TRUE FALSE
%token PRINTLN PRINT
%token EQ NEQ LE GE AND OR INC DEC

%token <ival> INTEGER_LITERAL
%token <fval> FLOAT_LITERAL
%token <sval> STRING_LITERAL CHAR_LITERAL IDENTIFIER

%type <node> program class_declaration class_body class_member main_method compound_statement statement_list statement
%type <node> variable_declaration assignment_statement print_statement expression data_type condition
%type <node> assignment_expression return_statement if_statement while_statement do_while_statement for_statement for_init

%left OR
%left AND
%left EQ NEQ
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UMINUS
%right '='
%right '!'

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%

program
    : class_declaration
    {
        $$ = new ASTNode("Program");
        $$->addChild($1);
        root = $$;
    }
    ;

class_declaration
    : PUBLIC CLASS IDENTIFIER '{' class_body '}'
    {
        $$ = new ASTNode("Class", $3);
        $$->addChild($5);
    }
    | CLASS IDENTIFIER '{' class_body '}'
    {
        $$ = new ASTNode("Class", $2);
        $$->addChild($4);
    }
    ;

class_body
    : class_body class_member
    {
        $$ = $1;
        if($2 != nullptr) $$->addChild($2);
    }
    | /* empty */
    {
        $$ = new ASTNode("ClassBody");
    }
    ;

class_member
    : main_method { $$ = $1; }
    | variable_declaration { $$ = $1; }
    ;

main_method
    : PUBLIC STATIC VOID MAIN '(' STRING '[' ']' IDENTIFIER ')' compound_statement
    {
        $$ = new ASTNode("MainMethod");
        $$->addChild($11);
    }
    ;

compound_statement
    : '{' statement_list '}'
    {
        $$ = new ASTNode("CompoundStatement");
        $$->addChild($2);
    }
    ;

statement_list
    : statement_list statement
    {
        $$ = $1;
        if($2 != nullptr) $$->addChild($2);
    }
    | /* empty */
    {
        $$ = new ASTNode("StatementList");
    }
    ;

statement
    : variable_declaration { $$ = $1; }
    | assignment_statement { $$ = $1; }
    | print_statement { $$ = $1; }
    | compound_statement { $$ = $1; }
    | if_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | do_while_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | ';' { $$ = nullptr; }
    ;

return_statement
    : RETURN expression ';'
    {
        $$ = new ASTNode("Return");
        $$->addChild($2);
    }
    | RETURN ';'
    {
        $$ = new ASTNode("Return");
    }
    ;

variable_declaration
    : data_type IDENTIFIER ';'
    {
        $$ = new ASTNode("VariableDeclaration");
        $$->addChild($1);
        $$->addChild(new ASTNode("Identifier", $2));
    }
    | data_type IDENTIFIER '=' expression ';'
    {
        $$ = new ASTNode("VariableDeclaration");
        $$->addChild($1);
        $$->addChild(new ASTNode("Identifier", $2));
        $$->addChild($4);
    }
    ;

data_type
    : INT { $$ = new ASTNode("Type", "int"); }
    | FLOAT { $$ = new ASTNode("Type", "float"); }
    | DOUBLE { $$ = new ASTNode("Type", "double"); }
    | CHAR { $$ = new ASTNode("Type", "char"); }
    | BOOLEAN { $$ = new ASTNode("Type", "boolean"); }
    | STRING { $$ = new ASTNode("Type", "String"); }
    ;

assignment_statement
    : IDENTIFIER '=' expression ';'
    {
        $$ = new ASTNode("Assignment");
        $$->addChild(new ASTNode("Identifier", $1));
        $$->addChild($3);
    }
    | IDENTIFIER INC ';'
    {
        $$ = new ASTNode("PostIncrement");
        $$->addChild(new ASTNode("Identifier", $1));
    }
    | IDENTIFIER DEC ';'
    {
        $$ = new ASTNode("PostDecrement");
        $$->addChild(new ASTNode("Identifier", $1));
    }
    ;

assignment_expression
    : IDENTIFIER '=' expression
    {
        $$ = new ASTNode("Assignment");
        $$->addChild(new ASTNode("Identifier", $1));
        $$->addChild($3);
    }
    | IDENTIFIER INC
    {
        $$ = new ASTNode("PostIncrement");
        $$->addChild(new ASTNode("Identifier", $1));
    }
    | IDENTIFIER DEC
    {
        $$ = new ASTNode("PostDecrement");
        $$->addChild(new ASTNode("Identifier", $1));
    }
    ;

print_statement
    : PRINTLN '(' expression ')' ';'
    {
        $$ = new ASTNode("Print");
        $$->addChild($3);
    }
    | PRINTLN '(' ')' ';'
    {
        $$ = new ASTNode("Print");
    }
    | PRINT '(' expression ')' ';'
    {
        $$ = new ASTNode("Print");
        $$->addChild($3);
    }
    ;

expression
    : expression '+' expression { $$ = new ASTNode("+"); $$->addChild($1); $$->addChild($3); }
    | expression '-' expression { $$ = new ASTNode("-"); $$->addChild($1); $$->addChild($3); }
    | expression '*' expression { $$ = new ASTNode("*"); $$->addChild($1); $$->addChild($3); }
    | expression '/' expression { $$ = new ASTNode("/"); $$->addChild($1); $$->addChild($3); }
    | expression '%' expression { $$ = new ASTNode("%"); $$->addChild($1); $$->addChild($3); }
    | '+' expression { $$ = $2; }
    | '-' expression %prec UMINUS { $$ = new ASTNode("UnaryMinus"); $$->addChild($2); }
    | '(' expression ')' { $$ = $2; }
    | IDENTIFIER { $$ = new ASTNode("Identifier", $1); }
    | INTEGER_LITERAL { $$ = new ASTNode("Integer", to_string($1)); }
    | FLOAT_LITERAL { $$ = new ASTNode("Float", to_string($1)); }
    | STRING_LITERAL { $$ = new ASTNode("String", $1); }
    | CHAR_LITERAL { $$ = new ASTNode("Char", $1); }
    | TRUE { $$ = new ASTNode("Boolean", "true"); }
    | FALSE { $$ = new ASTNode("Boolean", "false"); }
    | IDENTIFIER INC { $$ = new ASTNode("PostIncrement"); $$->addChild(new ASTNode("Identifier", $1)); }
    | IDENTIFIER DEC { $$ = new ASTNode("PostDecrement"); $$->addChild(new ASTNode("Identifier", $1)); }
    | INC IDENTIFIER { $$ = new ASTNode("PreIncrement"); $$->addChild(new ASTNode("Identifier", $2)); }
    | DEC IDENTIFIER { $$ = new ASTNode("PreDecrement"); $$->addChild(new ASTNode("Identifier", $2)); }
    ;

condition
    : expression '<' expression { $$ = new ASTNode("<"); $$->addChild($1); $$->addChild($3); }
    | expression '>' expression { $$ = new ASTNode(">"); $$->addChild($1); $$->addChild($3); }
    | expression LE expression { $$ = new ASTNode("<="); $$->addChild($1); $$->addChild($3); }
    | expression GE expression { $$ = new ASTNode(">="); $$->addChild($1); $$->addChild($3); }
    | expression EQ expression { $$ = new ASTNode("=="); $$->addChild($1); $$->addChild($3); }
    | expression NEQ expression { $$ = new ASTNode("!="); $$->addChild($1); $$->addChild($3); }
    | condition AND condition { $$ = new ASTNode("&&"); $$->addChild($1); $$->addChild($3); }
    | condition OR condition { $$ = new ASTNode("||"); $$->addChild($1); $$->addChild($3); }
    | '!' condition { $$ = new ASTNode("!"); $$->addChild($2); }
    | '(' condition ')' { $$ = $2; }
    ;

if_statement
    : IF '(' condition ')' statement %prec LOWER_THAN_ELSE
    {
        $$ = new ASTNode("If");
        $$->addChild($3);
        $$->addChild($5);
    }
    | IF '(' condition ')' statement ELSE statement
    {
        $$ = new ASTNode("IfElse");
        $$->addChild($3);
        $$->addChild($5);
        $$->addChild($7);
    }
    ;

while_statement
    : WHILE '(' condition ')' statement
    {
        $$ = new ASTNode("While");
        $$->addChild($3);
        $$->addChild($5);
    }
    ;

do_while_statement
    : DO statement WHILE '(' condition ')' ';'
    {
        $$ = new ASTNode("DoWhile");
        $$->addChild($2);
        $$->addChild($5);
    }
    ;

for_init
    : data_type IDENTIFIER '=' expression
    {
        $$ = new ASTNode("VariableDeclaration");
        $$->addChild($1);
        $$->addChild(new ASTNode("Identifier", $2));
        $$->addChild($4);
    }
    | assignment_expression
    {
        $$ = $1;
    }
    ;

for_statement
    : FOR '(' for_init ';' condition ';' assignment_expression ')' statement
    {
        $$ = new ASTNode("For");
        $$->addChild($3);
        $$->addChild($5);
        $$->addChild($7);
        $$->addChild($9);
    }
    ;

%%

void yyerror(const char *s)
{
    syntaxErrorOccurred = true;
    cerr << "Syntax Error at line " << yylineno << ": " << s << endl;
}

int main()
{
    yyparse();

    if (syntaxErrorOccurred) {
        cerr << "Compilation aborted due to syntax errors." << endl;
        return 1;
    }

    // Print Phase 1
    printTokenTable();

    // Print Phase 2
    cout << "\n=======================================================\n";
    cout << "           PHASE 2: SYNTAX ANALYSIS (AST)              \n";
    cout << "=======================================================\n";
    printAST(root);
    cout << "=======================================================\n";

    // Print Phase 3
    semanticCheck(root);
    symbolTable.print();

    // Print Phase 4
    resetICG();
    generateTAC(root);
    printTAC();

    // Print Phase 5
    optimizeTAC(tacInstructions);
    printOptimizedTAC();

    // Print Phase 6
    executeAST(root);
    printExecutionOutput();

    // Output JSON block for Node.js API consumer
    cout << "\n===JSON_START===\n{";
    
    // Tokens JSON
    cout << "\"tokens\":[";
    for (size_t i = 0; i < tokenList.size(); i++) {
        cout << "{\"line\":" << tokenList[i].line
             << ",\"type\":\"" << escapeJSON(tokenList[i].type) << "\""
             << ",\"lexeme\":\"" << escapeJSON(tokenList[i].lexeme) << "\"}";
        if (i + 1 < tokenList.size()) cout << ",";
    }
    cout << "],";

    // Symbol Table JSON
    cout << "\"symbolTable\":[";
    int symIdx = 0;
    for (auto item : symbolTable.getTable()) {
        cout << "{\"name\":\"" << escapeJSON(item.first) << "\",\"type\":\"" << escapeJSON(item.second) << "\"}";
        if (++symIdx < (int)symbolTable.getTable().size()) cout << ",";
    }
    cout << "],";

    // Semantic Output JSON
    cout << "\"semanticLogs\":[";
    for (size_t i = 0; i < semanticLogs.size(); i++) {
        cout << "\"" << escapeJSON(semanticLogs[i]) << "\"";
        if (i + 1 < semanticLogs.size()) cout << ",";
    }
    cout << "],";

    // TAC JSON
    cout << "\"tac\":[";
    for (size_t i = 0; i < tacInstructions.size(); i++) {
        const auto& t = tacInstructions[i];
        cout << "{\"op\":\"" << escapeJSON(t.op) << "\""
             << ",\"arg1\":\"" << escapeJSON(t.arg1) << "\""
             << ",\"arg2\":\"" << escapeJSON(t.arg2) << "\""
             << ",\"result\":\"" << escapeJSON(t.result) << "\""
             << ",\"comment\":\"" << escapeJSON(t.comment) << "\"}";
        if (i + 1 < tacInstructions.size()) cout << ",";
    }
    cout << "],";

    // Optimized TAC JSON
    cout << "\"optimizedTac\":[";
    for (size_t i = 0; i < optimizedTac.size(); i++) {
        const auto& t = optimizedTac[i];
        cout << "{\"op\":\"" << escapeJSON(t.op) << "\""
             << ",\"arg1\":\"" << escapeJSON(t.arg1) << "\""
             << ",\"arg2\":\"" << escapeJSON(t.arg2) << "\""
             << ",\"result\":\"" << escapeJSON(t.result) << "\""
             << ",\"comment\":\"" << escapeJSON(t.comment) << "\"}";
        if (i + 1 < optimizedTac.size()) cout << ",";
    }
    cout << "],";

    // Execution Output JSON
    cout << "\"executionOutput\":[";
    for (size_t i = 0; i < executionOutput.size(); i++) {
        cout << "\"" << escapeJSON(executionOutput[i]) << "\"";
        if (i + 1 < executionOutput.size()) cout << ",";
    }
    cout << "]\n";
    cout << "}\n===JSON_END===\n";

    return 0;
}
