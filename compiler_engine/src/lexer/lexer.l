%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "parser.tab.hpp"
#include "../error/error_handler.h"
extern int yylineno;
%}

%option noyywrap
%option yylineno

DIGIT   [0-9]
LETTER  [a-zA-Z_]

%%

"int"        { return INT_KW; }
"float"      { return FLOAT_KW; }
"if"         { return IF_KW; }
"else"       { return ELSE_KW; }
"while"      { return WHILE_KW; }
"printf"     { return PRINTF_KW; }

{DIGIT}+"."{DIGIT}+   { yylval.fval = atof(yytext); return FLOAT_LIT; }
{DIGIT}+              { yylval.ival = atoi(yytext); return INT_LIT; }

\"([^"\\]|\\.)*\"     { yylval.sval = strdup(yytext); return STRING_LIT; }

{LETTER}({LETTER}|{DIGIT})*  { yylval.sval = strdup(yytext); return IDENTIFIER; }

"+"   { return PLUS; }
"-"   { return MINUS; }
"*"   { return MUL; }
"/"   { return DIV; }
"%"   { return MOD; }
"<="  { return LE; }
">="  { return GE; }
"=="  { return EQ; }
"!="  { return NE; }
"<"   { return LT; }
">"   { return GT; }
"&&"  { return AND; }
"||"  { return OR; }
"!"   { return NOT; }
"="   { return ASSIGN; }
"{"   { return LBRACE; }
"}"   { return RBRACE; }
"("   { return LPAREN; }
")"   { return RPAREN; }
";"   { return SEMI; }
","   { return COMMA; }

"//".*                       { /* discard */ }
"/*"([^*]|\*+[^*/])*\*+"/"   { /* discard */ }
[ \t\r]+                     { /* discard */ }
\n                            { /* discard */ }

.   { ErrorHandler::reportLexicalError(yylineno, yytext); }

%%