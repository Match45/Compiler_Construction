%{

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "parser.tab.hpp"

extern int yylineno;
using namespace std;

%}

%option noyywrap
%option yylineno

DIGIT   [0-9]
LETTER  [a-zA-Z_]

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
"break"         { return KEYWORD_BREAK; }
"continue"      { return KEYWORD_CONTINUE; }
"cout"          { return KEYWORD_COUT; }
"cin"           { return KEYWORD_CIN; }
"endl"          { return KEYWORD_ENDL; }
"return"        { return KEYWORD_RETURN; }

{LETTER}({LETTER}|{DIGIT})*     { yylval.str = new std::string(yytext); return IDENTIFIER; }

{DIGIT}+"."{DIGIT}+             { yylval.dval = atof(yytext); return NUMBER; }

{DIGIT}+                        { yylval.dval = atof(yytext); return NUMBER; }

\"([^\"\\]|\\.)*\"              { yylval.str = new std::string(yytext); return STRING_LITERAL; }

"=="|"!="|"<="|">="|"<"|">"     { yylval.str = new std::string(yytext); return RELATIONAL; }

"//".*                      { }

"/*"([^*]|\*+[^*/])*\*+"/"  { }

"<<"        { return LEFT_SHIFT; }
">>"        { return RIGHT_SHIFT; }

"="         { return ASSIGNMENT; }

"+"         { return '+'; }
"-"         { return '-'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }

"++"        { return INCREMENT; }
"--"        { return DECREMENT; }

"&&"        { return LOGICAL_AND; }
"||"        { return LOGICAL_OR; }
"!"         { return LOGICAL_NOT; }

";"         { return SEMICOLON; }
","         { return COMMA; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"("         { return LPAREN; }
")"         { return RPAREN; }

[ \t\r]+    { }

\n          { }

. {
    cout << "Lexer Warning on Line " << yylineno << ": Unknown character -> " << yytext << endl;
}

%%