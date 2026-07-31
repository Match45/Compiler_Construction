%code requires {
    #include <string>
    #include <vector>
    #include "ast.h"
}

%{
#include <iostream>
#include <cstdio>
#include "ast.h"

using namespace std;

int yylex();
void yyerror(const char *s);

extern int yylineno;
extern FILE *yyin;

ASTNode *g_ast_root = nullptr;  

static void report(const char *msg) { cout << msg << " parsed.\n"; }

%}

%union {
    ASTNode                  *node;
    std::vector<ASTNode*>    *list;
    std::vector<std::string> *strlist;
    std::string              *str;
    double                   dval;
}

/* Keywords */
%token KEYWORD_NAMESPACE KEYWORD_USING
%token KEYWORD_INT KEYWORD_FLOAT KEYWORD_DOUBLE KEYWORD_CHAR KEYWORD_VOID
%token KEYWORD_IF KEYWORD_ELSE
%token KEYWORD_WHILE KEYWORD_FOR 
%token KEYWORD_BREAK KEYWORD_CONTINUE KEYWORD_RETURN
%token KEYWORD_COUT KEYWORD_CIN KEYWORD_ENDL

/* Literals / identifiers */
%token <str> IDENTIFIER STRING_LITERAL
%token <dval> NUMBER

/* Operators */
%token ASSIGNMENT RELATIONAL
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token LEFT_SHIFT RIGHT_SHIFT
%token INCREMENT DECREMENT

/* Punctuation */
%token SEMICOLON COMMA
%token LBRACE RBRACE
%token LPAREN RPAREN

/* Nonterminal types */
%type <node> program global_item namespace_stmt function_def parameter
%type <node> compound_stmt statement declaration assignment_stmt
%type <node> if_stmt while_stmt for_stmt for_init for_update
%type <node> return_stmt break_stmt continue_stmt cout_stmt cin_stmt
%type <node> function_call_stmt expression cout_item
%type <str>  type_spec
%type <list> parameter_list parameters statement_list cout_chain argument_list
%type <strlist> cin_chain

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
      {
          $$ = new ProgramNode();
          g_ast_root = $$;
      }

    | program global_item
      {
          if ($2) $1->children.push_back($2); 
          $$ = $1; g_ast_root = $1; 
      }
    ;

global_item
    : namespace_stmt  { $$ = nullptr; }
    | function_def    { $$ = $1; }
    ;

namespace_stmt
    : KEYWORD_USING KEYWORD_NAMESPACE IDENTIFIER SEMICOLON
      { 
        cout << "Using namespace parsed.\n";
        delete $3;
        $$ = nullptr; 
      }
    ;

/* ---------- Types ---------- */

type_spec
    : KEYWORD_INT      { $$ = new string("int"); }
    | KEYWORD_FLOAT    { $$ = new string("float"); }
    | KEYWORD_DOUBLE   { $$ = new string("double"); }
    | KEYWORD_CHAR     { $$ = new string("char"); }
    | KEYWORD_VOID     { $$ = new string("void"); }
    ;

/* ---------- Functions ---------- */

function_def
    : type_spec IDENTIFIER LPAREN parameter_list RPAREN compound_stmt
      {
          $$ = makeFuncDef(*$1, *$2, $4, $6, yylineno);
          delete $1; delete $2;
          cout << "Function definition parsed.\n";
      }
    ;

parameter_list
    : /* empty */    { $$ = new vector<ASTNode*>(); }
    | parameters     { $$ = $1; }
    ;

parameters
    : parameter                   { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
    | parameters COMMA parameter  { $1->push_back($3); $$ = $1; }
    ;

parameter
    : type_spec IDENTIFIER
      {
          $$ = new ParamNode(*$1, *$2);
          delete $1; delete $2;
      }
    ;

/* ---------- Statements ---------- */

compound_stmt
    : LBRACE statement_list RBRACE  { $$ = makeCompound($2); }

    ;

statement_list
    : /* empty */                 { $$ = new std::vector<ASTNode*>(); }
    | statement_list statement    { $1->push_back($2); $$ = $1; }
    ;

statement
    : declaration          { $$ = $1; }
    | assignment_stmt      { $$ = $1; }
    | if_stmt              { $$ = $1; }
    | while_stmt           { $$ = $1; }
    | for_stmt             { $$ = $1; }
    | return_stmt          { $$ = $1; }
    | break_stmt           { $$ = $1; }
    | continue_stmt        { $$ = $1; }
    | cin_stmt             { $$ = $1; }
    | cout_stmt            { $$ = $1; }
    | function_call_stmt   { $$ = $1; }
    | compound_stmt        { $$ = $1; }
    ;

declaration
    : type_spec IDENTIFIER SEMICOLON                       { $$ = makeVarDecl(*$1, *$2, nullptr, yylineno); delete $1; delete $2; report("Variable declaration"); }
    | type_spec IDENTIFIER ASSIGNMENT expression SEMICOLON { $$ = makeVarDecl(*$1, *$2, $4, yylineno); delete $1; delete $2; report("Variable initialization"); }
    ;

assignment_stmt
    : IDENTIFIER ASSIGNMENT expression SEMICOLON   { $$ = makeAssign(*$1, $3, yylineno); delete $1; report("Assignment"); }
    | IDENTIFIER INCREMENT SEMICOLON               { $$ = makeIncDec(*$1, true, yylineno); delete $1; report("Increment"); }
    | IDENTIFIER DECREMENT SEMICOLON               { $$ = makeIncDec(*$1, false, yylineno); delete $1; report("Decrement"); }
    ;

/* ---------- Conditional ---------- */

if_stmt
    : KEYWORD_IF LPAREN expression RPAREN compound_stmt                            
        { $$ = makeIf($3, $5, nullptr, yylineno); cout << "If statement parsed.\n"; }
    | KEYWORD_IF LPAREN expression RPAREN compound_stmt KEYWORD_ELSE compound_stmt
        { $$ = makeIf($3, $5, $7, yylineno); cout << "If-Else statement parsed.\n"; }
    | KEYWORD_IF LPAREN expression RPAREN compound_stmt KEYWORD_ELSE if_stmt
        { $$ = makeIf($3, $5, $7, yylineno); cout << "If-Else-If statement parsed.\n"; }
    ;

/* ---------- Loops ---------- */

while_stmt
    : KEYWORD_WHILE LPAREN expression RPAREN compound_stmt
        { $$ = makeWhile($3, $5, yylineno); report("While loop"); }
    ;

for_stmt
    : KEYWORD_FOR LPAREN for_init SEMICOLON expression SEMICOLON for_update RPAREN compound_stmt
        { $$ = makeFor($3, $5, $7, $9, yylineno); report("For loop"); }
    ;

for_init
    : /* empty */                                  { $$ = nullptr; }
    | type_spec IDENTIFIER ASSIGNMENT expression   { $$ = makeVarDecl(*$1, *$2, $4, yylineno); delete $1; delete $2; }
    | IDENTIFIER ASSIGNMENT expression             { $$ = makeAssign(*$1, $3, yylineno); delete $1; }
    ;

for_update
    : /* empty */                      { $$ = nullptr; }
    | IDENTIFIER ASSIGNMENT expression { $$ = makeAssign(*$1, $3, yylineno); delete $1; }
    | IDENTIFIER INCREMENT             { $$ = makeIncDec(*$1, true, yylineno); delete $1; }
    | IDENTIFIER DECREMENT             { $$ = makeIncDec(*$1, false, yylineno); delete $1; }
    ;

/* ---------- Jump statements ---------- */

return_stmt
    : KEYWORD_RETURN expression SEMICOLON
        { $$ = makeReturn($2, yylineno); report("Return statement"); }
    | KEYWORD_RETURN SEMICOLON             
        { $$ = makeReturn(nullptr, yylineno); report("Void return"); }
    ;


break_stmt
    : KEYWORD_BREAK SEMICOLON    { $$ = makeBreak(yylineno); report("Break"); }
    ;

continue_stmt
    : KEYWORD_CONTINUE SEMICOLON { $$ = makeContinue(yylineno); report("Continue"); }
    ;

/* ---------- I/O ---------- */
cin_stmt
    : KEYWORD_CIN cin_chain SEMICOLON
        { $$ = makeCin($2, yylineno); report("Cin statement"); }
    ;
cin_chain
    : RIGHT_SHIFT IDENTIFIER            { $$ = new vector<string>(); $$->push_back(*$2); delete $2; }
    | cin_chain RIGHT_SHIFT IDENTIFIER  { $1->push_back(*$3); delete $3; $$ = $1; }
    ;

cout_stmt
    : KEYWORD_COUT cout_chain SEMICOLON
        { $$ = makeCout($2, yylineno); report("Cout statement"); }
    ;

cout_chain
    : LEFT_SHIFT cout_item              { $$ = new vector<ASTNode*>(); $$->push_back($2); }
    | cout_chain LEFT_SHIFT cout_item   { $1->push_back($3); $$ = $1; }
    ;

cout_item 
    : expression   { $$ = $1; } 
    | KEYWORD_ENDL { $$ = makeEndl(); }
    ;

/* ---------- Function calls as statements ---------- */

function_call_stmt
    : IDENTIFIER LPAREN argument_list RPAREN SEMICOLON { $$ = makeCall(*$1, $3, yylineno); delete $1; report("Function call"); }
    | IDENTIFIER LPAREN RPAREN SEMICOLON               { $$ = makeCall(*$1, nullptr, yylineno); delete $1; report("Function call (no arguments)"); }
    ;

argument_list
    : expression                     { $$ = new vector<ASTNode*>(); $$->push_back($1); }
    | argument_list COMMA expression { $1->push_back($3); $$ = $1; }
    ;

expression
    : expression '+' expression              { $$ = makeBinOp("+", $1, $3, yylineno); }
    | expression '-' expression              { $$ = makeBinOp("-", $1, $3, yylineno); }
    | expression '*' expression              { $$ = makeBinOp("*", $1, $3, yylineno); }
    | expression '/' expression              { $$ = makeBinOp("/", $1, $3, yylineno); }
    | expression '%' expression              { $$ = makeBinOp("%", $1, $3, yylineno); }
    | expression RELATIONAL expression       { $$ = makeBinOp(*$2, $1, $3, yylineno); delete $2; }
    | expression LOGICAL_AND expression      { $$ = makeBinOp("&&", $1, $3, yylineno); }
    | expression LOGICAL_OR expression       { $$ = makeBinOp("||", $1, $3, yylineno); }
    | LOGICAL_NOT expression                 { $$ = makeUnaryOp("!", $2, yylineno); }
    | '-' expression %prec UMINUS            { $$ = makeUnaryOp("-", $2, yylineno); }
    | LPAREN expression RPAREN               { $$ = $2; }
    | IDENTIFIER LPAREN argument_list RPAREN { $$ = makeCall(*$1, $3, yylineno); delete $1; }
    | IDENTIFIER LPAREN RPAREN               { $$ = makeCall(*$1, nullptr, yylineno); delete $1; }
    | IDENTIFIER                             { $$ = makeIdNode(*$1, yylineno); delete $1; }
    | NUMBER                                 { $$ = makeNumberNode($1, yylineno); }
    | STRING_LITERAL                         { $$ = makeStringNode(*$1, yylineno); delete $1; }
    ;

%%

void yyerror(const char *s) { cout << "Syntax Error on line " << yylineno << ": " << s << endl; }

int main(int argc, char *argv[])
{
    if (argc > 1) 
    {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) 
        {   
            cerr << "Cannot open input file." << endl; return 1; 
        }
    } 
    else {
        cout << "Enter C++ program (Ctrl+D to finish):" << endl;
    }

    if (yyparse() == 0) 
    { 
        cout << "Parsing completed successfully.\n"; printAST(g_ast_root); 
    }
    else { 
        cout << "Parsing failed.\n";
    }

    if (yyin != stdin && yyin != NULL) 
    fclose(yyin);
    
    return 0;
}

