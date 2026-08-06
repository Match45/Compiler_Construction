%{

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "ast.h"
#include "../Semantic/semantic.h"
#include "../IntermediateCode/icg.h"
#include "../Optimizer/optimizer.h"
#include "../TargetCode/virtual_machine.h"

using namespace std;

int yylex();
void yyerror(const char *s);

extern int yylineno;
extern FILE *yyin;

ProgramNode* rootAST = nullptr;
bool hasSyntaxError = false;

%}

%union {
    char* str_val;
    ASTNode* node_val;
    std::vector<ASTNode*>* vec_val;
    std::vector<Parameter>* param_vec_val;
    Parameter* param_val;
    CoutItem* cout_item_val;
    std::vector<CoutItem>* cout_vec_val;
    std::vector<std::string>* str_vec_val;
}

/* Keywords */
%token KEYWORD_NAMESPACE KEYWORD_USING
%token <str_val> KEYWORD_INT KEYWORD_FLOAT KEYWORD_DOUBLE KEYWORD_CHAR KEYWORD_VOID
%token KEYWORD_IF KEYWORD_ELSE
%token KEYWORD_WHILE KEYWORD_FOR KEYWORD_DO
%token KEYWORD_BREAK KEYWORD_CONTINUE KEYWORD_RETURN
%token KEYWORD_COUT KEYWORD_CIN KEYWORD_ENDL

/* Literals / identifiers */
%token <str_val> IDENTIFIER STRING_LITERAL NUMBER

/* Operators */
%token ASSIGNMENT 
%token <str_val> RELATIONAL
%token LOGICAL_AND LOGICAL_OR LOGICAL_NOT
%token LEFT_SHIFT RIGHT_SHIFT
%token INCREMENT DECREMENT

/* Punctuation */
%token SEMICOLON COMMA
%token LBRACE RBRACE
%token LPAREN RPAREN

/* Nonterminal types */
%type <node_val> program global_item namespace_stmt function_def compound_stmt statement
%type <node_val> declaration assignment_stmt if_stmt while_stmt for_stmt do_while_stmt
%type <node_val> return_stmt break_stmt continue_stmt cout_stmt cin_stmt function_call_stmt expression
%type <str_val> type_spec declaration_no_semicolon
%type <vec_val> statement_list argument_list
%type <param_vec_val> parameter_list parameters
%type <param_val> parameter
%type <cout_vec_val> cout_chain
%type <str_vec_val> cin_chain

/* Precedence (lowest to highest) */
%left LOGICAL_OR
%left LOGICAL_AND
%left RELATIONAL
%left '+' '-'
%left '*' '/' '%'
%right LOGICAL_NOT
%right UMINUS
%right ASSIGNMENT
%right INCREMENT DECREMENT

%%

/* ---------- Program ---------- */

program
    : /* empty */
      {
          rootAST = new ProgramNode(yylineno);
          $$ = rootAST;
      }
    | program global_item
      {
          if ($2 != nullptr) {
              rootAST->statements.push_back($2);
          }
          $$ = rootAST;
      }
    ;

global_item
    : namespace_stmt      { $$ = nullptr; }
    | function_def        { $$ = $1; }
    | declaration         { $$ = $1; }
    ;

namespace_stmt
    : KEYWORD_USING KEYWORD_NAMESPACE IDENTIFIER SEMICOLON
      {
          $$ = nullptr;
      }
    ;

type_spec
    : KEYWORD_INT    { $$ = $1; }
    | KEYWORD_FLOAT  { $$ = $1; }
    | KEYWORD_DOUBLE { $$ = $1; }
    | KEYWORD_CHAR   { $$ = $1; }
    | KEYWORD_VOID   { $$ = $1; }
    ;

function_def
    : type_spec IDENTIFIER LPAREN parameter_list RPAREN compound_stmt
      {
          std::vector<Parameter> params;
          if ($4 != nullptr) {
              params = *$4;
              delete $4;
          }
          $$ = new FunctionDefNode($1, $2, params, $6, yylineno);
      }
    ;

parameter_list
    : /* empty */
      {
          $$ = new std::vector<Parameter>();
      }
    | parameters
      {
          $$ = $1;
      }
    ;

parameters
    : parameter
      {
          $$ = new std::vector<Parameter>();
          $$->push_back(*$1);
          delete $1;
      }
    | parameters COMMA parameter
      {
          $1->push_back(*$3);
          delete $3;
          $$ = $1;
      }
    ;

parameter
    : type_spec IDENTIFIER
      {
          $$ = new Parameter{$1, $2};
      }
    ;

compound_stmt
    : LBRACE statement_list RBRACE
      {
          BlockNode* block = new BlockNode(yylineno);
          if ($2 != nullptr) {
              block->statements = *$2;
              delete $2;
          }
          $$ = block;
      }
    ;

statement_list
    : /* empty */
      {
          $$ = new std::vector<ASTNode*>();
      }
    | statement_list statement
      {
          if ($2 != nullptr) {
              $1->push_back($2);
          }
          $$ = $1;
      }
    ;

statement
    : declaration           { $$ = $1; }
    | assignment_stmt       { $$ = $1; }
    | if_stmt               { $$ = $1; }
    | while_stmt            { $$ = $1; }
    | for_stmt              { $$ = $1; }
    | do_while_stmt         { $$ = $1; }
    | return_stmt           { $$ = $1; }
    | break_stmt            { $$ = $1; }
    | continue_stmt         { $$ = $1; }
    | cout_stmt             { $$ = $1; }
    | cin_stmt              { $$ = $1; }
    | function_call_stmt    { $$ = $1; }
    | compound_stmt         { $$ = $1; }
    ;

declaration
    : type_spec IDENTIFIER SEMICOLON
      {
          $$ = new VarDeclNode($1, $2, nullptr, yylineno);
      }
    | type_spec IDENTIFIER ASSIGNMENT expression SEMICOLON
      {
          $$ = new VarDeclNode($1, $2, $4, yylineno);
      }
    ;

assignment_stmt
    : IDENTIFIER ASSIGNMENT expression SEMICOLON
      {
          $$ = new AssignNode($1, $3, yylineno);
      }
    | IDENTIFIER INCREMENT SEMICOLON
      {
          ASTNode* idNode = new IdentifierNode($1, yylineno);
          ASTNode* oneNode = new LiteralNode("int", "1", yylineno);
          ASTNode* addNode = new BinaryExprNode("+", idNode, oneNode, yylineno);
          $$ = new AssignNode($1, addNode, yylineno);
      }
    | IDENTIFIER DECREMENT SEMICOLON
      {
          ASTNode* idNode = new IdentifierNode($1, yylineno);
          ASTNode* oneNode = new LiteralNode("int", "1", yylineno);
          ASTNode* subNode = new BinaryExprNode("-", idNode, oneNode, yylineno);
          $$ = new AssignNode($1, subNode, yylineno);
      }
    ;

if_stmt
    : KEYWORD_IF LPAREN expression RPAREN statement
      {
          $$ = new IfStmtNode($3, $5, nullptr, yylineno);
      }
    | KEYWORD_IF LPAREN expression RPAREN statement KEYWORD_ELSE statement
      {
          $$ = new IfStmtNode($3, $5, $7, yylineno);
      }
    ;

while_stmt
    : KEYWORD_WHILE LPAREN expression RPAREN statement
      {
          $$ = new WhileStmtNode($3, $5, yylineno);
      }
    ;

for_stmt
    : KEYWORD_FOR LPAREN declaration_no_semicolon SEMICOLON expression SEMICOLON IDENTIFIER INCREMENT RPAREN statement
      {
          ASTNode* init = new VarDeclNode("int", $3, nullptr, yylineno);
          ASTNode* idNode = new IdentifierNode($7, yylineno);
          ASTNode* oneNode = new LiteralNode("int", "1", yylineno);
          ASTNode* addNode = new BinaryExprNode("+", idNode, oneNode, yylineno);
          ASTNode* update = new AssignNode($7, addNode, yylineno);

          $$ = new ForStmtNode(init, $5, update, $10, yylineno);
      }
    | KEYWORD_FOR LPAREN IDENTIFIER ASSIGNMENT expression SEMICOLON expression SEMICOLON IDENTIFIER INCREMENT RPAREN statement
      {
          ASTNode* init = new AssignNode($3, $5, yylineno);
          ASTNode* idNode = new IdentifierNode($9, yylineno);
          ASTNode* oneNode = new LiteralNode("int", "1", yylineno);
          ASTNode* addNode = new BinaryExprNode("+", idNode, oneNode, yylineno);
          ASTNode* update = new AssignNode($9, addNode, yylineno);

          $$ = new ForStmtNode(init, $7, update, $12, yylineno);
      }
    ;

declaration_no_semicolon
    : type_spec IDENTIFIER
      {
          $$ = $2;
      }
    | type_spec IDENTIFIER ASSIGNMENT expression
      {
          $$ = $2;
      }
    ;

do_while_stmt
    : KEYWORD_DO compound_stmt KEYWORD_WHILE LPAREN expression RPAREN SEMICOLON
      {
          $$ = new WhileStmtNode($5, $2, yylineno);
      }
    ;

return_stmt
    : KEYWORD_RETURN expression SEMICOLON
      {
          $$ = new ReturnNode($2, yylineno);
      }
    | KEYWORD_RETURN SEMICOLON
      {
          $$ = new ReturnNode(nullptr, yylineno);
      }
    ;

break_stmt
    : KEYWORD_BREAK SEMICOLON { $$ = nullptr; }
    ;

continue_stmt
    : KEYWORD_CONTINUE SEMICOLON { $$ = nullptr; }
    ;

cout_stmt
    : KEYWORD_COUT cout_chain SEMICOLON
      {
          CoutNode* node = new CoutNode(yylineno);
          if ($2 != nullptr) {
              node->items = *$2;
              delete $2;
          }
          $$ = node;
      }
    ;

cout_chain
    : LEFT_SHIFT expression
      {
          $$ = new std::vector<CoutItem>();
          $$->push_back(CoutItem{$2, false});
      }
    | LEFT_SHIFT KEYWORD_ENDL
      {
          $$ = new std::vector<CoutItem>();
          $$->push_back(CoutItem{nullptr, true});
      }
    | cout_chain LEFT_SHIFT expression
      {
          $1->push_back(CoutItem{$3, false});
          $$ = $1;
      }
    | cout_chain LEFT_SHIFT KEYWORD_ENDL
      {
          $1->push_back(CoutItem{nullptr, true});
          $$ = $1;
      }
    ;

cin_stmt
    : KEYWORD_CIN cin_chain SEMICOLON
      {
          CinNode* node = new CinNode(yylineno);
          if ($2 != nullptr) {
              node->varNames = *$2;
              delete $2;
          }
          $$ = node;
      }
    ;

cin_chain
    : RIGHT_SHIFT IDENTIFIER
      {
          $$ = new std::vector<std::string>();
          $$->push_back($2);
      }
    | cin_chain RIGHT_SHIFT IDENTIFIER
      {
          $1->push_back($3);
          $$ = $1;
      }
    ;

function_call_stmt
    : IDENTIFIER LPAREN argument_list RPAREN SEMICOLON
      {
          std::vector<ASTNode*> args;
          if ($3 != nullptr) {
              args = *$3;
              delete $3;
          }
          $$ = new FuncCallNode($1, args, yylineno);
      }
    | IDENTIFIER LPAREN RPAREN SEMICOLON
      {
          $$ = new FuncCallNode($1, std::vector<ASTNode*>(), yylineno);
      }
    ;

argument_list
    : expression
      {
          $$ = new std::vector<ASTNode*>();
          $$->push_back($1);
      }
    | argument_list COMMA expression
      {
          $1->push_back($3);
          $$ = $1;
      }
    ;

expression
    : expression '+' expression
      {
          $$ = new BinaryExprNode("+", $1, $3, yylineno);
      }
    | expression '-' expression
      {
          $$ = new BinaryExprNode("-", $1, $3, yylineno);
      }
    | expression '*' expression
      {
          $$ = new BinaryExprNode("*", $1, $3, yylineno);
      }
    | expression '/' expression
      {
          $$ = new BinaryExprNode("/", $1, $3, yylineno);
      }
    | expression '%' expression
      {
          $$ = new BinaryExprNode("%", $1, $3, yylineno);
      }
    | expression RELATIONAL expression
      {
          $$ = new BinaryExprNode($2, $1, $3, yylineno);
      }
    | expression LOGICAL_AND expression
      {
          $$ = new BinaryExprNode("&&", $1, $3, yylineno);
      }
    | expression LOGICAL_OR expression
      {
          $$ = new BinaryExprNode("||", $1, $3, yylineno);
      }
    | LOGICAL_NOT expression
      {
          $$ = new UnaryExprNode("!", $2, yylineno);
      }
    | '-' expression %prec UMINUS
      {
          $$ = new UnaryExprNode("-", $2, yylineno);
      }
    | LPAREN expression RPAREN
      {
          $$ = $2;
      }
    | IDENTIFIER
      {
          $$ = new IdentifierNode($1, yylineno);
      }
    | NUMBER
      {
          $$ = new LiteralNode("int", $1, yylineno);
      }
    | STRING_LITERAL
      {
          $$ = new LiteralNode("string", $1, yylineno);
      }
    | IDENTIFIER LPAREN argument_list RPAREN
      {
          std::vector<ASTNode*> args;
          if ($3 != nullptr) {
              args = *$3;
              delete $3;
          }
          $$ = new FuncCallNode($1, args, yylineno);
      }
    | IDENTIFIER LPAREN RPAREN
      {
          $$ = new FuncCallNode($1, std::vector<ASTNode*>(), yylineno);
      }
    ;

%%

void yyerror(const char *s)
{
    cerr << "Syntax Error on Line " << yylineno << ": " << s << endl;
    hasSyntaxError = true;
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            cerr << "Cannot open input file: " << argv[1] << endl;
            return 1;
        }
    }

    if (yyparse() != 0 || hasSyntaxError || rootAST == nullptr)
    {
        cerr << "Compilation failed during Lexical/Syntax Analysis." << endl;
        if (yyin != stdin && yyin != NULL) fclose(yyin);
        return 1;
    }

    SemanticAnalyzer semantic;
    if (!semantic.analyze(rootAST))
    {
        for (const auto& err : semantic.getErrors())
        {
            cerr << err << endl;
        }
        cerr << "Compilation failed during Semantic Analysis." << endl;
        delete rootAST;
        if (yyin != stdin && yyin != NULL) fclose(yyin);
        return 1;
    }

    IntermediateCodeGenerator icg;
    auto rawTAC = icg.generate(rootAST);

    CodeOptimizer optimizer;
    auto optTAC = optimizer.optimize(rawTAC);

    VirtualMachine vm;
    if (!vm.execute(optTAC))
    {
        cerr << "Program execution terminated with runtime error." << endl;
        delete rootAST;
        if (yyin != stdin && yyin != NULL) fclose(yyin);
        return 1;
    }

    delete rootAST;
    if (yyin != stdin && yyin != NULL) fclose(yyin);
    
    return 0;
}
