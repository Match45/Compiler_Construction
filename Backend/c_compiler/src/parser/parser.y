%code requires {
#include "../src/ast/ast.h"
}
%{
#include <cstdio>
#include <cstdlib>
#include "../src/ast/ast.h"

int yylex();
void yyerror(const char *s);
extern int yylineno;
ASTNode* root = nullptr;
%}

%union {
    int intval;
    float floatval;
    char* strval;
    ASTNode* node;
}

%token INT_T FLOAT_T CHAR_T DOUBLE_T VOID_T
%token IF ELSE WHILE FOR RETURN BREAK CONTINUE
%token <strval> ID STRING_CONST CHAR_CONST
%token <intval> INT_CONST
%token <floatval> FLOAT_CONST
%token EQ NEQ LE GE AND OR INC DEC

%type <node> program decl_list decl func_def param_list params
%type <node> type compound_stmt stmt_list stmt expr_stmt
%type <node> if_stmt while_stmt for_stmt return_stmt
%type <node> expr assign_expr or_expr and_expr eq_expr rel_expr
%type <node> add_expr mul_expr unary_expr primary_expr
%type <node> arg_list args var_decl

%right '='
%left OR
%left AND
%left EQ NEQ
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right '!' UMINUS

%%

program:
    decl_list { root = makeNode(NODE_PROGRAM, "program", yylineno, $1, nullptr); }
    ;

decl_list:
    decl_list decl        { $$ = appendSibling($1, $2); }
    | decl                { $$ = $1; }
    ;

decl:
    func_def   { $$ = $1; }
    | var_decl { $$ = $1; }
    ;

type:
      INT_T    { $$ = makeTypeNode("int"); }
    | FLOAT_T  { $$ = makeTypeNode("float"); }
    | CHAR_T   { $$ = makeTypeNode("char"); }
    | DOUBLE_T { $$ = makeTypeNode("double"); }
    | VOID_T   { $$ = makeTypeNode("void"); }
    ;

var_decl:
      type ID ';'
        { $$ = makeVarDecl($1, $2, nullptr, yylineno); }
    | type ID '=' expr ';'
        { $$ = makeVarDecl($1, $2, $4, yylineno); }
    | type ID '[' INT_CONST ']' ';'
        { $$ = makeArrayDecl($1, $2, $4, yylineno); }
    ;

func_def:
    type ID '(' param_list ')' compound_stmt
        { $$ = makeFuncDef($1, $2, $4, $6, yylineno); }
    ;

param_list:
      /* empty */ { $$ = nullptr; }
    | params      { $$ = $1; }
    ;

params:
      type ID              { $$ = makeParam($1, $2); }
    | params ',' type ID   { $$ = appendSibling($1, makeParam($3, $4)); }
    ;

compound_stmt:
    '{' stmt_list '}' { $$ = makeNode(NODE_BLOCK, "block", yylineno, $2, nullptr); }
    ;

stmt_list:
      /* empty */         { $$ = nullptr; }
    | stmt_list stmt       { $$ = appendSibling($1, $2); }
    ;

stmt:
      var_decl        { $$ = $1; }
    | expr_stmt        { $$ = $1; }
    | compound_stmt     { $$ = $1; }
    | if_stmt           { $$ = $1; }
    | while_stmt         { $$ = $1; }
    | for_stmt            { $$ = $1; }
    | return_stmt          { $$ = $1; }
    | BREAK ';'    { $$ = makeNode(NODE_BREAK, "break", yylineno, nullptr, nullptr); }
    | CONTINUE ';' { $$ = makeNode(NODE_CONTINUE, "continue", yylineno, nullptr, nullptr); }
    ;

expr_stmt:
    expr ';' { $$ = $1; }
    ;

if_stmt:
      IF '(' expr ')' stmt
        { $$ = makeIf($3, $5, nullptr, yylineno); }
    | IF '(' expr ')' stmt ELSE stmt
        { $$ = makeIf($3, $5, $7, yylineno); }
    ;

while_stmt:
    WHILE '(' expr ')' stmt { $$ = makeWhile($3, $5, yylineno); }
    ;

for_stmt:
    FOR '(' expr_stmt expr ';' expr ')' stmt
        { $$ = makeFor($3, $4, $6, $8, yylineno); }
    ;

return_stmt:
      RETURN ';'      { $$ = makeReturn(nullptr, yylineno); }
    | RETURN expr ';' { $$ = makeReturn($2, yylineno); }
    ;

expr: assign_expr { $$ = $1; };

assign_expr:
      ID '=' assign_expr { $$ = makeAssign($1, $3, yylineno); }
    | or_expr             { $$ = $1; }
    ;

or_expr:  or_expr OR and_expr  { $$ = makeBinOp("||", $1, $3, yylineno); } | and_expr { $$=$1; };
and_expr: and_expr AND eq_expr { $$ = makeBinOp("&&", $1, $3, yylineno); } | eq_expr { $$=$1; };
eq_expr:  eq_expr EQ rel_expr  { $$ = makeBinOp("==", $1, $3, yylineno); }
        | eq_expr NEQ rel_expr { $$ = makeBinOp("!=", $1, $3, yylineno); }
        | rel_expr { $$=$1; };
rel_expr: rel_expr '<' add_expr { $$ = makeBinOp("<", $1, $3, yylineno); }
        | rel_expr '>' add_expr { $$ = makeBinOp(">", $1, $3, yylineno); }
        | rel_expr LE add_expr  { $$ = makeBinOp("<=", $1, $3, yylineno); }
        | rel_expr GE add_expr  { $$ = makeBinOp(">=", $1, $3, yylineno); }
        | add_expr { $$=$1; };
add_expr: add_expr '+' mul_expr { $$ = makeBinOp("+", $1, $3, yylineno); }
        | add_expr '-' mul_expr { $$ = makeBinOp("-", $1, $3, yylineno); }
        | mul_expr { $$=$1; };
mul_expr: mul_expr '*' unary_expr { $$ = makeBinOp("*", $1, $3, yylineno); }
        | mul_expr '/' unary_expr { $$ = makeBinOp("/", $1, $3, yylineno); }
        | mul_expr '%' unary_expr { $$ = makeBinOp("%", $1, $3, yylineno); }
        | unary_expr { $$=$1; };

unary_expr:
      '-' unary_expr %prec UMINUS { $$ = makeUnaryOp("-", $2, yylineno); }
    | '!' unary_expr              { $$ = makeUnaryOp("!", $2, yylineno); }
    | primary_expr                { $$ = $1; }
    ;

primary_expr:
      ID                      { $$ = makeIdNode($1, yylineno); }
    | INT_CONST                { $$ = makeIntNode($1, yylineno); }
    | FLOAT_CONST                { $$ = makeFloatNode($1, yylineno); }
    | STRING_CONST                { $$ = makeStringNode($1, yylineno); }
    | CHAR_CONST                   { $$ = makeCharNode($1, yylineno); }
    | ID '[' expr ']'               { $$ = makeArrayAccess($1, $3, yylineno); }
    | ID '(' arg_list ')'            { $$ = makeCall($1, $3, yylineno); }
    | '(' expr ')'                    { $$ = $2; }
    ;

arg_list: /* empty */ { $$ = nullptr; } | args { $$ = $1; };
args: expr { $$ = $1; } | args ',' expr { $$ = appendSibling($1, $3); };

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error: %s at line %d\n", s, yylineno);
}