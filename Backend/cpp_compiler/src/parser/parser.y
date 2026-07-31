%{
#include <iostream>
#include <cstdio>

using namespace std;

int yylex();
void yyerror(const char *s);

extern int yylineno;
extern FILE *yyin;
%}

%union {
    int    ival;
    double dval;
    char  *sval;
}

/* Keywords */
%token KEYWORD_NAMESPACE KEYWORD_USING
%token KEYWORD_INT KEYWORD_FLOAT KEYWORD_DOUBLE KEYWORD_CHAR KEYWORD_VOID
%token KEYWORD_IF KEYWORD_ELSE
%token KEYWORD_WHILE KEYWORD_FOR KEYWORD_DO
%token KEYWORD_BREAK KEYWORD_CONTINUE KEYWORD_RETURN
%token KEYWORD_COUT KEYWORD_CIN KEYWORD_ENDL

/* Literals / identifiers */
%token <sval> IDENTIFIER
%token <ival> NUMBER
%token <sval> STRING_LITERAL

/* Operators */
%token ASSIGNMENT RELATIONAL
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token LEFT_SHIFT RIGHT_SHIFT
%token INCREMENT DECREMENT

/* Punctuation */
%token SEMICOLON COMMA
%token LBRACE RBRACE
%token LPAREN RPAREN

/* Precedence (lowest to highest) */
%right ASSIGNMENT
%left  LOGICAL_OR
%left  LOGICAL_AND
%right LOGICAL_NOT
%left  RELATIONAL
%left  '+' '-'
%left  '*' '/' '%'
%right UMINUS

%%

/* ---------- Program ---------- */

program
    : /* empty */
    | program global_item
    ;

global_item
    : namespace_stmt
    | function_def
    ;

namespace_stmt
    : KEYWORD_USING KEYWORD_NAMESPACE IDENTIFIER SEMICOLON
      { cout << "Using namespace parsed.\n"; }
    ;

/* ---------- Types ---------- */

type_spec
    : KEYWORD_INT
    | KEYWORD_FLOAT
    | KEYWORD_DOUBLE
    | KEYWORD_CHAR
    | KEYWORD_VOID
    ;

/* ---------- Functions ---------- */

function_def
    : type_spec IDENTIFIER LPAREN parameter_list RPAREN compound_stmt
      { cout << "Function definition parsed.\n"; }
    ;

parameter_list
    : /* empty */
    | parameters
    ;

parameters
    : parameter
    | parameters COMMA parameter
    ;

parameter
    : type_spec IDENTIFIER
    ;

/* ---------- Statements ---------- */

compound_stmt
    : LBRACE statement_list RBRACE
    ;

statement_list
    : /* empty */
    | statement_list statement
    ;

statement
    : declaration
    | assignment_stmt
    | if_stmt
    | while_stmt
    | for_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
    | cin_stmt
    | cout_stmt
    | function_call_stmt
    | compound_stmt
    ;

declaration
    : type_spec IDENTIFIER SEMICOLON
      { cout << "Variable declaration parsed.\n"; }
    | type_spec IDENTIFIER ASSIGNMENT expression SEMICOLON
      { cout << "Variable initialization parsed.\n"; }
    ;

assignment_stmt
    : IDENTIFIER ASSIGNMENT expression SEMICOLON
      { cout << "Assignment parsed.\n"; }
    | IDENTIFIER INCREMENT SEMICOLON
      { cout << "Increment parsed.\n"; }
    | IDENTIFIER DECREMENT SEMICOLON
      { cout << "Decrement parsed.\n"; }
    ;

/* ---------- Conditional ---------- */

if_stmt
    : KEYWORD_IF LPAREN expression RPAREN compound_stmt
      { cout << "If statement parsed.\n"; }
    | KEYWORD_IF LPAREN expression RPAREN compound_stmt KEYWORD_ELSE compound_stmt
      { cout << "If-Else statement parsed.\n"; }
    | KEYWORD_IF LPAREN expression RPAREN compound_stmt KEYWORD_ELSE if_stmt
      { cout << "If-Else-If statement parsed.\n"; }
    ;

/* ---------- Loops ---------- */

while_stmt
    : KEYWORD_WHILE LPAREN expression RPAREN compound_stmt
      { cout << "While loop parsed.\n"; }
    ;

for_stmt
    : KEYWORD_FOR LPAREN for_init SEMICOLON expression SEMICOLON for_update RPAREN compound_stmt
      { cout << "For loop parsed.\n"; }
    ;

for_init
    : /* empty */
    | type_spec IDENTIFIER ASSIGNMENT expression
    | IDENTIFIER ASSIGNMENT expression
    ;

for_update
    : /* empty */
    | IDENTIFIER ASSIGNMENT expression
    | IDENTIFIER INCREMENT
    | IDENTIFIER DECREMENT
    ;

/* ---------- Jump statements ---------- */

return_stmt
    : KEYWORD_RETURN expression SEMICOLON
      { cout << "Return statement parsed.\n"; }
    | KEYWORD_RETURN SEMICOLON
      { cout << "Void return parsed.\n"; }
    ;

break_stmt
    : KEYWORD_BREAK SEMICOLON
      { cout << "Break parsed.\n"; }
    ;

continue_stmt
    : KEYWORD_CONTINUE SEMICOLON
      { cout << "Continue parsed.\n"; }
    ;

/* ---------- I/O ---------- */
cin_stmt
    : KEYWORD_CIN cin_chain SEMICOLON
      { cout << "Cin statement parsed.\n"; }
    ;

cin_chain
    : RIGHT_SHIFT IDENTIFIER
    | cin_chain RIGHT_SHIFT IDENTIFIER
    ;

cout_stmt
    : KEYWORD_COUT cout_chain SEMICOLON
      { cout << "Cout statement parsed.\n"; }
    ;

cout_chain
    : LEFT_SHIFT cout_item
    | cout_chain LEFT_SHIFT cout_item
    ;

cout_item
    : expression
    | KEYWORD_ENDL
    ;

/* ---------- Function calls as statements ---------- */

function_call_stmt
    : IDENTIFIER LPAREN argument_list RPAREN SEMICOLON
      { cout << "Function call parsed.\n"; }
    | IDENTIFIER LPAREN RPAREN SEMICOLON
      { cout << "Function call (no arguments) parsed.\n"; }
    ;

argument_list
    : expression
    | argument_list COMMA expression
    ;

/* ---------- Expressions ---------- */

expression
    : expression '+' expression          { cout << "Addition parsed.\n"; }
    | expression '-' expression          { cout << "Subtraction parsed.\n"; }
    | expression '*' expression          { cout << "Multiplication parsed.\n"; }
    | expression '/' expression          { cout << "Division parsed.\n"; }
    | expression '%' expression          { cout << "Modulo parsed.\n"; }
    | expression RELATIONAL expression   { cout << "Relational expression parsed.\n"; }
    | expression LOGICAL_AND expression  { cout << "Logical AND parsed.\n"; }
    | expression LOGICAL_OR expression   { cout << "Logical OR parsed.\n"; }
    | LOGICAL_NOT expression             { cout << "Logical NOT parsed.\n"; }
    | '-' expression %prec UMINUS        { cout << "Unary minus parsed.\n"; }
    | LPAREN expression RPAREN
    | IDENTIFIER LPAREN argument_list RPAREN { cout << "Function call expression parsed.\n"; }
    | IDENTIFIER LPAREN RPAREN               { cout << "Function call expression parsed.\n"; }
    | IDENTIFIER
    | NUMBER
    | STRING_LITERAL
    ;

%%

void yyerror(const char *s)
{
    cout << "Syntax Error on line " << yylineno << ": " << s << endl;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            cerr << "Cannot open input file." << endl;
            return 1;
        }
    } else {
        cout << "Enter C++ program (Ctrl+D to finish):" << endl;
    }

    if (yyparse() == 0)
        cout << "Parsing completed successfully.\n";
    else
        cout << "Parsing failed.\n";

    if (yyin != stdin && yyin != NULL)
        fclose(yyin);

    return 0;
}
