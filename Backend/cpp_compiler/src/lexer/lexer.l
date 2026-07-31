%{

#include <iostream>
#include <cstdio>
#include "parser.tab.h"

using namespace std;

%}

%option noyywrap
%option yylineno

%%

"#".*     { /* Ignore preprocessor directives */ }

"namespace"     { return KEYWORD_NAMESPACE; }
"using"         { return KEYWORD_USING; }
"int"           { return KEYWORD_INT; }
"float"         { return KEYWORD_FLOAT; }
"double"        { return KEYWORD_DOUBLE; }
"char"          { return KEYWORD_CHAR; }
"void"          { return KEYWORD_VOID; }
"if"            { return KEYWORD_IF; }
"else"          { return KEYWORD_ELSE; }
"while"         { return KEYWORD_WHILE; }
"for"           { return KEYWORD_FOR; }
"do"            { return KEYWORD_DO; }
"break"         { return KEYWORD_BREAK; }
"continue"      { return KEYWORD_CONTINUE; }
"cout"          { return KEYWORD_COUT; }
"cin"           { return KEYWORD_CIN; }
"endl"          { return KEYWORD_ENDL; }
"return"        { return KEYWORD_RETURN; }

"//".*                      { }

"/*"([^*]|\*+[^*/])*\*+"/"  { }

\"([^\\\"]|\\.)*\"          { return STRING_LITERAL; }

[0-9]+(\.[0-9]+)?           { return NUMBER; }

[a-zA-Z_][a-zA-Z0-9_]*      { return IDENTIFIER; }

"<<"        { return LEFT_SHIFT; }
">>"        { return RIGHT_SHIFT; }

"="         { return ASSIGNMENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }

"&"         { return '&'; }

"++"        { return INCREMENT; }
"--"        { return DECREMENT; }

"=="        { return RELATIONAL; }
"!="        { return RELATIONAL; }
"<="        { return RELATIONAL; }
">="        { return RELATIONAL; }
"<"         { return RELATIONAL; }
">"         { return RELATIONAL; }

"&&"        { return LOGICAL_AND; }
"||"        { return LOGICAL_OR; }
"!"         { return LOGICAL_NOT; }

";"         { return SEMICOLON; }
","         { return COMMA; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"["         { return '['; }
"]"         { return ']'; }
":"         { return ':'; }

[ \t\r]+    { }

\n          { }

. {
    cout << "Lexer Warning on Line " << yylineno << ": Unknown character -> " << yytext << endl;
}

%%