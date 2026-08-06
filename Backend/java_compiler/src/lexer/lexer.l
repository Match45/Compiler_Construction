%{
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#include "../parser/parser.tab.h"

extern int yylineno;
%}

%option noyywrap
%option yylineno

LETTER      [a-zA-Z_]
DIGIT       [0-9]

ID              {LETTER}({LETTER}|{DIGIT})*
INTEGERNUM      {DIGIT}+
FLOATNUM        {DIGIT}+"."{DIGIT}+

STRING_PATTERN  \"([^\\\"]|\\.)*\"
CHAR_PATTERN    \'([^\\\']|\\.)\'

%%

"public"                  { return PUBLIC; }
"private"                 { return PRIVATE; }
"protected"               { return PROTECTED; }

"class"                   { return CLASS; }

"static"                  { return STATIC; }

"void"                    { return VOID; }

"main"                    { return MAIN; }

"int"                     { return INT; }
"float"                   { return FLOAT; }
"double"                  { return DOUBLE; }
"char"                    { return CHAR; }
"boolean"                 { return BOOLEAN; }
"String"                  { return STRING; }

"if"                      { return IF; }
"else"                    { return ELSE; }

"for"                     { return FOR; }
"while"                   { return WHILE; }
"do"                      { return DO; }

"return"                  { return RETURN; }

"true"                    { return TRUE; }
"false"                   { return FALSE; }

"System.out.println"      { return PRINTLN; }

"=="                      { return EQ; }
"!="                      { return NEQ; }
"<="                      { return LE; }
">="                      { return GE; }

"&&"                      { return AND; }
"||"                      { return OR; }

"++"                      { return INC; }
"--"                      { return DEC; }

"+"                       { return '+'; }
"-"                       { return '-'; }
"*"                       { return '*'; }
"/"                       { return '/'; }
"%"                       { return '%'; }

"="                       { return '='; }

"<"                       { return '<'; }
">"                       { return '>'; }

"!"                       { return '!'; }

";"                       { return ';'; }
","                       { return ','; }

"("                       { return '('; }
")"                       { return ')'; }

"{"                       { return '{'; }
"}"                       { return '}'; }

"["                       { return '['; }
"]"                       { return ']'; }

{FLOATNUM} {
    yylval.fval = atof(yytext);
    return FLOAT_LITERAL;
}

{INTEGERNUM} {
    yylval.ival = atoi(yytext);
    return INTEGER_LITERAL;
}

{STRING_PATTERN} {
    yylval.sval = strdup(yytext);
    return STRING_LITERAL;
}

{CHAR_PATTERN} {
    yylval.sval = strdup(yytext);
    return CHAR_LITERAL;
}

{ID} {
    yylval.sval = strdup(yytext);
    return IDENTIFIER;
}

"//".*      { }

[ \t\r]+    { }

\n          { }

. {
    cout << "Lexical Error at line "
         << yylineno
         << " : "
         << yytext
         << endl;
}

%%