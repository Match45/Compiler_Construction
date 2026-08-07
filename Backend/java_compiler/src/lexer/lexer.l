%{
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "tokens.h"
#include "../parser/parser.tab.h"

using namespace std;

extern int yylineno;

static char* copyString(const char* s) {
    char* d = (char*)malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}
%}

%option noyywrap
%option yylineno
%option never-interactive

LETTER      [a-zA-Z_]
DIGIT       [0-9]

ID              {LETTER}({LETTER}|{DIGIT})*
INTEGERNUM      {DIGIT}+
FLOATNUM        {DIGIT}+"."{DIGIT}+

STRING_PATTERN  \"([^\\\"]|\\.)*\"
CHAR_PATTERN    \'([^\\\']|\\.)\'

%%

"public"                  { addToken("KEYWORD", yytext, yylineno); return PUBLIC; }
"private"                 { addToken("KEYWORD", yytext, yylineno); return PRIVATE; }
"protected"               { addToken("KEYWORD", yytext, yylineno); return PROTECTED; }

"class"                   { addToken("KEYWORD", yytext, yylineno); return CLASS; }
"static"                  { addToken("KEYWORD", yytext, yylineno); return STATIC; }
"void"                    { addToken("KEYWORD", yytext, yylineno); return VOID; }
"main"                    { addToken("KEYWORD", yytext, yylineno); return MAIN; }

"int"                     { addToken("DATA_TYPE", yytext, yylineno); return INT; }
"float"                   { addToken("DATA_TYPE", yytext, yylineno); return FLOAT; }
"double"                  { addToken("DATA_TYPE", yytext, yylineno); return DOUBLE; }
"char"                    { addToken("DATA_TYPE", yytext, yylineno); return CHAR; }
"boolean"                 { addToken("DATA_TYPE", yytext, yylineno); return BOOLEAN; }
"String"                  { addToken("DATA_TYPE", yytext, yylineno); return STRING; }

"if"                      { addToken("KEYWORD", yytext, yylineno); return IF; }
"else"                    { addToken("KEYWORD", yytext, yylineno); return ELSE; }
"for"                     { addToken("KEYWORD", yytext, yylineno); return FOR; }
"while"                   { addToken("KEYWORD", yytext, yylineno); return WHILE; }
"do"                      { addToken("KEYWORD", yytext, yylineno); return DO; }
"return"                  { addToken("KEYWORD", yytext, yylineno); return RETURN; }

"true"                    { addToken("BOOLEAN_LITERAL", yytext, yylineno); return TRUE; }
"false"                   { addToken("BOOLEAN_LITERAL", yytext, yylineno); return FALSE; }

"System.out.println"      { addToken("PRINT_STMT", yytext, yylineno); return PRINTLN; }
"System.out.print"        { addToken("PRINT_STMT", yytext, yylineno); return PRINT; }

"=="                      { addToken("OPERATOR", yytext, yylineno); return EQ; }
"!="                      { addToken("OPERATOR", yytext, yylineno); return NEQ; }
"<="                      { addToken("OPERATOR", yytext, yylineno); return LE; }
">="                      { addToken("OPERATOR", yytext, yylineno); return GE; }

"&&"                      { addToken("OPERATOR", yytext, yylineno); return AND; }
"||"                      { addToken("OPERATOR", yytext, yylineno); return OR; }

"++"                      { addToken("OPERATOR", yytext, yylineno); return INC; }
"--"                      { addToken("OPERATOR", yytext, yylineno); return DEC; }

"+"                       { addToken("OPERATOR", yytext, yylineno); return '+'; }
"-"                       { addToken("OPERATOR", yytext, yylineno); return '-'; }
"*"                       { addToken("OPERATOR", yytext, yylineno); return '*'; }
"/"                       { addToken("OPERATOR", yytext, yylineno); return '/'; }
"%"                       { addToken("OPERATOR", yytext, yylineno); return '%'; }

"="                       { addToken("OPERATOR", yytext, yylineno); return '='; }

"<"                       { addToken("OPERATOR", yytext, yylineno); return '<'; }
">"                       { addToken("OPERATOR", yytext, yylineno); return '>'; }

"!"                       { addToken("OPERATOR", yytext, yylineno); return '!'; }

";"                       { addToken("DELIMITER", yytext, yylineno); return ';'; }
","                       { addToken("DELIMITER", yytext, yylineno); return ','; }

"("                       { addToken("DELIMITER", yytext, yylineno); return '('; }
")"                       { addToken("DELIMITER", yytext, yylineno); return ')'; }

"{"                       { addToken("DELIMITER", yytext, yylineno); return '{'; }
"}"                       { addToken("DELIMITER", yytext, yylineno); return '}'; }

"["                       { addToken("DELIMITER", yytext, yylineno); return '['; }
"]"                       { addToken("DELIMITER", yytext, yylineno); return ']'; }

{FLOATNUM} {
    addToken("FLOAT_LITERAL", yytext, yylineno);
    yylval.fval = atof(yytext);
    return FLOAT_LITERAL;
}

{INTEGERNUM} {
    addToken("INTEGER_LITERAL", yytext, yylineno);
    yylval.ival = atoi(yytext);
    return INTEGER_LITERAL;
}

{STRING_PATTERN} {
    addToken("STRING_LITERAL", yytext, yylineno);
    yylval.sval = copyString(yytext);
    return STRING_LITERAL;
}

{CHAR_PATTERN} {
    addToken("CHAR_LITERAL", yytext, yylineno);
    yylval.sval = copyString(yytext);
    return CHAR_LITERAL;
}

{ID} {
    addToken("IDENTIFIER", yytext, yylineno);
    yylval.sval = copyString(yytext);
    return IDENTIFIER;
}

"//".*                    { /* ignore inline comments */ }
"/*"([^*]|\*+[^/*])*\*+"/" { /* ignore block comments */ }

[ \t\r]+                  { }
\n                        { }

. {
    cout << "Lexical Error at line " << yylineno << " : " << yytext << endl;
}

%%
