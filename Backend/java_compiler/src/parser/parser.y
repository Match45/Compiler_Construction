%{

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "../ast/ast.h"

using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *s);

ASTNode *root;

%} 

%union{

    int ival;
    float fval;
    char *sval;
    ASTNode *node;


}

%token PUBLIC
%token PRIVATE
%token PROTECTED

%token CLASS
%token STATIC
%token VOID
%token MAIN

%token INT
%token FLOAT
%token DOUBLE
%token CHAR
%token BOOLEAN
%token STRING

%token IF
%token ELSE
%token FOR
%token WHILE
%token DO
%token RETURN

%token TRUE
%token FALSE

%token PRINTLN

%token EQ
%token NEQ
%token LE
%token GE

%token AND
%token OR

%token INC
%token DEC

%token <ival> INTEGER_LITERAL
%token <fval> FLOAT_LITERAL
%token <sval> STRING_LITERAL
%token <sval> CHAR_LITERAL

%token <sval> IDENTIFIER

%type <node> program
%type <node> class_declaration
%type <node> class_body
%type <node> class_member
%type <node> main_method
%type <node> compound_statement
%type <node> statement_list
%type <node> statement
%type <node> variable_declaration
%type <node> assignment_statement
%type <node> print_statement
%type <node> expression

%type <node> data_type
%type <node> assignment_expression


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

        cout << endl;
        cout << "Java Parsing Successful" << endl;

        cout << endl;
        cout << ".......... AST .........." << endl;

        printAST(root);
    }
    ;

class_declaration
    : PUBLIC CLASS IDENTIFIER
      '{'
      class_body
      '}'
    {
        $$ = new ASTNode("Class", $3);

        $$->addChild($5);
    }
    ;

class_body
    : class_body class_member
    {
        $$ = $1;

        if($2 != nullptr)
            $$->addChild($2);
    }

    | /* empty */
    {
        $$ = new ASTNode("ClassBody");
    }
    ;

class_member
    : main_method
    {
        $$ = $1;
    }

    | variable_declaration
    {
        $$ = $1;
    }
    ;

main_method
    : PUBLIC STATIC VOID MAIN
      '('
      STRING
      '['
      ']'
      IDENTIFIER
      ')'
      compound_statement
    {
        $$ = new ASTNode("MainMethod");

       $$->addChild($11);
    }
    ;

compound_statement
    : '{'
      statement_list
      '}'
    {
        $$ = new ASTNode("CompoundStatement");

        $$->addChild($2);
    }
    ;

statement_list
    : statement_list statement
    {
        $$ = $1;

        if($2 != nullptr)
            $$->addChild($2);
    }

    | /* empty */
    {
        $$ = new ASTNode("StatementList");
    }
    ;

statement
    : variable_declaration
    {
        $$ = $1;
    }

    | assignment_statement
    {
        $$ = $1;
    }

    | print_statement
    {
        $$ = $1;
    }

    | compound_statement
    {
        $$ = $1;
    }

    | if_statement
    {
        $$ = nullptr;
    }

    | while_statement
    {
        $$ = nullptr;
    }

    | for_statement
    {
        $$ = nullptr;
    }

    | return_statement
    {
        $$ = nullptr;
    }

    | ';'
    {
        $$ = nullptr;
    }
    ;

return_statement
    : RETURN expression ';'
    | RETURN ';'
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
    : INT
    {
        $$ = new ASTNode("Type", "int");
    }

    | FLOAT
    {
        $$ = new ASTNode("Type", "float");
    }

    | DOUBLE
    {
        $$ = new ASTNode("Type", "double");
    }

    | CHAR
    {
        $$ = new ASTNode("Type", "char");
    }

    | BOOLEAN
    {
        $$ = new ASTNode("Type", "boolean");
    }

    | STRING
    {
        $$ = new ASTNode("Type", "String");
    }
    ;

assignment_statement
    : IDENTIFIER '=' expression ';'
    {
        $$ = new ASTNode("Assignment");

        $$->addChild(new ASTNode("Identifier", $1));

        $$->addChild($3);
    }
    ;

assignment_expression
    : IDENTIFIER '=' expression
    {
        $$ = new ASTNode("Assignment");

        $$->addChild(new ASTNode("Identifier", $1));

        $$->addChild($3);
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
    ;


expression
    : expression '+' expression
    {
        $$ = new ASTNode("+");
        $$->addChild($1);
        $$->addChild($3);
    }

    | expression '-' expression
    {
        $$ = new ASTNode("-");
        $$->addChild($1);
        $$->addChild($3);
    }

    | expression '*' expression
    {
        $$ = new ASTNode("*");
        $$->addChild($1);
        $$->addChild($3);
    }

    | expression '/' expression
    {
        $$ = new ASTNode("/");
        $$->addChild($1);
        $$->addChild($3);
    }

    | expression '%' expression
    {
        $$ = new ASTNode("%");
        $$->addChild($1);
        $$->addChild($3);
    }

    | '+' expression
    {
        $$ = $2;
    }

    | '-' expression %prec UMINUS
    {
        $$ = new ASTNode("UnaryMinus");
        $$->addChild($2);
    }

    | '(' expression ')'
    {
        $$ = $2;
    }

    | IDENTIFIER
    {
        $$ = new ASTNode("Identifier", $1);
    }

    | INTEGER_LITERAL
    {
        $$ = new ASTNode("Integer", to_string($1));
    }

    | FLOAT_LITERAL
    {
        $$ = new ASTNode("Float", to_string($1));
    }

    | STRING_LITERAL
    {
        $$ = new ASTNode("String", $1);
    }

    | CHAR_LITERAL
    {
        $$ = new ASTNode("Char", $1);
    }

    | TRUE
    {
        $$ = new ASTNode("Boolean", "true");
    }
    | FALSE
    {
        $$ = new ASTNode("Boolean", "false");
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

    | INC IDENTIFIER
    {
        $$ = new ASTNode("PreIncrement");
        $$->addChild(new ASTNode("Identifier", $2));
    }

    | DEC IDENTIFIER
    {
        $$ = new ASTNode("PreDecrement");
        $$->addChild(new ASTNode("Identifier", $2));
    }
    ;


condition
    : expression '<' expression
    | expression '>' expression
    | expression LE expression
    | expression GE expression
    | expression EQ expression
    | expression NEQ expression
    | condition AND condition
    | condition OR condition
    | '!' condition
    | '(' condition ')'
    ;


if_statement
    : IF '(' condition ')' statement
      %prec LOWER_THAN_ELSE
    | IF '(' condition ')' statement ELSE statement
    ;


while_statement
    : WHILE '(' condition ')' statement
    ;


for_statement

    :

      FOR

      '('

      assignment_expression

      ';'

      condition

      ';'

      assignment_expression

      ')'

      statement

    ;

%%


void yyerror(const char *s)
{

    cerr << endl;

    cerr << "Syntax Error at line "

         << yylineno

         << endl;

}


int main()
{

    yyparse();

    return 0;

}