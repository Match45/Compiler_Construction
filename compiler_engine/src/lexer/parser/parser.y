%code requires {
#include "ast.h"
}

%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast.h"
#include "../error/error_handler.h"

extern int yylineno;
int yylex();
void yyerror(const char* s);

ASTPtr programRoot;
%}

%union {
    int ival;
    double fval;
    char* sval;
    ASTNode* node;
}

%token <sval> IDENTIFIER STRING_LIT
%token <ival> INT_LIT
%token <fval> FLOAT_LIT

%token INT_KW FLOAT_KW IF_KW ELSE_KW WHILE_KW PRINTF_KW
%token PLUS MINUS MUL DIV MOD
%token LT GT LE GE EQ NE AND OR NOT
%token ASSIGN LBRACE RBRACE LPAREN RPAREN SEMI COMMA

%type <node> program stmt_list stmt expr block
%type <node> decl_stmt assign_stmt if_stmt while_stmt print_stmt

%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT UMINUS

%%

program:
    stmt_list {
        ASTNode* p = new ASTNode(NodeType::PROGRAM, 1);
        for (auto& c : $1->children) p->children.push_back(c);
        programRoot = ASTPtr(p);
        delete $1;
    }
    ;

stmt_list:
      /* empty */        { $$ = new ASTNode(NodeType::BLOCK, yylineno); }
    | stmt_list stmt      { $1->children.push_back(ASTPtr($2)); $$ = $1; }
    ;

stmt:
      decl_stmt SEMI  { $$ = $1; }
    | assign_stmt SEMI { $$ = $1; }
    | if_stmt            { $$ = $1; }
    | while_stmt          { $$ = $1; }
    | print_stmt SEMI   { $$ = $1; }
    | block                { $$ = $1; }
    ;

decl_stmt:
      INT_KW IDENTIFIER {
        ASTNode* n = new ASTNode(NodeType::VAR_DECL, yylineno);
        n->dataType = "int"; n->strValue = $2; free($2);
        $$ = n;
      }
    | FLOAT_KW IDENTIFIER {
        ASTNode* n = new ASTNode(NodeType::VAR_DECL, yylineno);
        n->dataType = "float"; n->strValue = $2; free($2);
        $$ = n;
      }
    ;

assign_stmt:
    IDENTIFIER ASSIGN expr {
        ASTNode* n = new ASTNode(NodeType::ASSIGN, yylineno);
        n->strValue = $1; free($1);
        n->children.push_back(ASTPtr($3));
        $$ = n;
    }
    ;

if_stmt:
      IF_KW LPAREN expr RPAREN block {
        ASTNode* n = new ASTNode(NodeType::IF_STMT, yylineno);
        n->children.push_back(ASTPtr($3));
        n->children.push_back(ASTPtr($5));
        $$ = n;
      }
    | IF_KW LPAREN expr RPAREN block ELSE_KW block {
        ASTNode* n = new ASTNode(NodeType::IF_STMT, yylineno);
        n->children.push_back(ASTPtr($3));
        n->children.push_back(ASTPtr($5));
        n->children.push_back(ASTPtr($7));
        $$ = n;
      }
    ;

while_stmt:
    WHILE_KW LPAREN expr RPAREN block {
        ASTNode* n = new ASTNode(NodeType::WHILE_STMT, yylineno);
        n->children.push_back(ASTPtr($3));
        n->children.push_back(ASTPtr($5));
        $$ = n;
    }
    ;

print_stmt:
    PRINTF_KW LPAREN STRING_LIT COMMA expr RPAREN {
        free($3);
        ASTNode* n = new ASTNode(NodeType::PRINT_STMT, yylineno);
        n->children.push_back(ASTPtr($5));
        $$ = n;
    }
    ;

block:
    LBRACE stmt_list RBRACE { $2->type = NodeType::BLOCK; $$ = $2; }
    ;

expr:
      expr PLUS expr  { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="+"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr MINUS expr { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="-"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr MUL expr   { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="*"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr DIV expr   { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="/"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr MOD expr   { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="%"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr LT expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="<"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr GT expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue=">"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr LE expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="<="; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr GE expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue=">="; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr EQ expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="=="; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr NE expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="!="; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr AND expr   { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="&&"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | expr OR expr    { ASTNode* n=new ASTNode(NodeType::BINARY_OP,yylineno); n->strValue="||"; n->children={ASTPtr($1),ASTPtr($3)}; $$=n; }
    | NOT expr        { ASTNode* n=new ASTNode(NodeType::UNARY_OP,yylineno); n->strValue="!"; n->children={ASTPtr($2)}; $$=n; }
    | MINUS expr %prec UMINUS { ASTNode* n=new ASTNode(NodeType::UNARY_OP,yylineno); n->strValue="-"; n->children={ASTPtr($2)}; $$=n; }
    | LPAREN expr RPAREN { $$ = $2; }
    | IDENTIFIER {
        ASTNode* n = new ASTNode(NodeType::IDENTIFIER, yylineno);
        n->strValue = $1; free($1);
        $$ = n;
      }
    | INT_LIT {
        ASTNode* n = new ASTNode(NodeType::LITERAL_INT, yylineno);
        n->intValue = $1;
        $$ = n;
      }
    | FLOAT_LIT {
        ASTNode* n = new ASTNode(NodeType::LITERAL_FLOAT, yylineno);
        n->floatValue = $1;
        $$ = n;
      }
    ;

%%

void yyerror(const char* s) {
    ErrorHandler::reportSyntaxError(yylineno, s);
}