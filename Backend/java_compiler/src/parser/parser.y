%{

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *s);

%}

%union{

    int ival;
    float fval;
    char *sval;

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
          cout << endl;
          cout << "                 " << endl;
          cout << " Java Parsing Successful " << endl;
          cout << "                 " << endl;
      }
    ;

class_declaration
    : PUBLIC CLASS IDENTIFIER
      '{'
      class_body
      '}'
    ;

class_body
    : class_body class_member
    | /* empty */
    ;

class_member
    : main_method
    | variable_declaration
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
    ;

compound_statement
    : '{'
      statement_list
      '}'
    ;

statement_list
    : statement_list statement
    | /* empty */
    ;

statement

    : variable_declaration

    | assignment_statement

    | print_statement

    | if_statement

    | while_statement

    | for_statement

    | return_statement

    | compound_statement

    | ';'

    ;

return_statement
    : RETURN expression ';'
    | RETURN ';'
    ;

variable_declaration
    : data_type IDENTIFIER ';'
    | data_type IDENTIFIER '=' expression ';'
    ;

data_type
    : INT
    | FLOAT
    | DOUBLE
    | CHAR
    | BOOLEAN
    | STRING
    ;


assignment_statement
    : IDENTIFIER '=' expression ';'
    ;

assignment_expression

    : IDENTIFIER '=' expression

    ;


print_statement

    : PRINTLN '(' expression ')' ';'

    | PRINTLN '(' ')' ';'

    ;


expression

    : expression '+' expression

    | expression '-' expression

    | expression '*' expression

    | expression '/' expression

    | expression '%' expression

    | '+' expression

    | '-' expression %prec UMINUS

    | '(' expression ')'

    | IDENTIFIER

    | INTEGER_LITERAL

    | FLOAT_LITERAL

    | STRING_LITERAL

    | CHAR_LITERAL

    | TRUE

    | FALSE

    | IDENTIFIER INC

    | IDENTIFIER DEC

    | INC IDENTIFIER

    | DEC IDENTIFIER

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