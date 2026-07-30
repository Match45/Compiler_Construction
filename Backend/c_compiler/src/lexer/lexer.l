%{
#include "parser.tab.h"
#include "../ast/ast.h"
#include <cstdlib>
#include <cstring>
extern int yylineno;
%}

%option noyywrap
%option yylineno

DIGIT     [0-9]
LETTER    [a-zA-Z_]
ID        {LETTER}({LETTER}|{DIGIT})*
INT       {DIGIT}+
FLOAT     {DIGIT}+"."{DIGIT}+
STRING    \"([^\\\"]|\\.)*\"
CHARLIT   \'([^\\\']|\\.)\'

%%

"//".*                         { /* line comment - skip */ }
"/*"([^*]|\*+[^*/])*\*+"/"     { /* block comment - skip */ }

"int"       { return INT_T; }
"float"     { return FLOAT_T; }
"char"      { return CHAR_T; }
"double"    { return DOUBLE_T; }
"void"      { return VOID_T; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"return"    { return RETURN; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }

{ID}        { yylval.strval = strdup(yytext); return ID; }
{FLOAT}     { yylval.floatval = atof(yytext); return FLOAT_CONST; }
{INT}       { yylval.intval = atoi(yytext); return INT_CONST; }
{STRING}    { yylval.strval = strdup(yytext); return STRING_CONST; }
{CHARLIT}   { yylval.strval = strdup(yytext); return CHAR_CONST; }

"=="        { return EQ; }
"!="        { return NEQ; }
"<="        { return LE; }
">="        { return GE; }
"&&"        { return AND; }
"||"        { return OR; }
"++"        { return INC; }
"--"        { return DEC; }

[-+*/%=<>!;,(){}\[\]]  { return yytext[0]; }

[ \t\r]+    { /* skip whitespace */ }
\n          { /* yylineno auto-increments */ }

.           { fprintf(stderr, "Lexical Error: unexpected char '%s' at line %d\n", yytext, yylineno); }

%%