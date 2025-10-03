%{
/* IMPROVED LEXICAL ANALYZER (SCANNER) WITH PRINTS
* Same format as old scanner, but prints tokens for debugging
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h" /* Token definitions from Bison */
int lex_debug = 0; /* set to 1 to enable token debug prints */
%}

/* Flex options */
%option nounput
%option noinput

/* Regular expression macros */
letter [a-zA-Z]
digit [0-9]
ID {letter}({letter}|{digit})*
TYPE (int|void|float|char|double|long|short)
KEYWORD (write|writeln|read)
NUMBER {digit}+(\.{digit}+)?([eE][+\-]?{digit}+)?

%%

{TYPE} { if (lex_debug) printf("%s : TYPE\n", yytext); return TYPE; }

{KEYWORD} { if (lex_debug) printf("%s : KEYWORD\n", yytext); return KEYWORD; }

"if"      { if (lex_debug) printf("%s : IF\n", yytext); return IF; }
"else"    { if (lex_debug) printf("%s : ELSE\n", yytext); return ELSE; }
"while"   { if (lex_debug) printf("%s : WHILE\n", yytext); return WHILE; }
"for"     { if (lex_debug) printf("%s : FOR\n", yytext); return FOR; }
"return"  { if (lex_debug) printf("%s : RETURN\n", yytext); return RETURN; }
"break"   { if (lex_debug) printf("%s : BREAK\n", yytext); return BREAK; }


"print" { if (lex_debug) printf("%s : PRINT\n", yytext); return PRINT; }

{ID} { if (lex_debug) printf("%s : IDENTIFIER\n", yytext); yylval.str = strdup(yytext); return ID; }

{NUMBER} { if (lex_debug) printf("%s : NUMBER\n", yytext); yylval.num = atof(yytext); return NUM; }


"(" { if (lex_debug) printf("%s : OPEN_PAREN\n", yytext); return '('; }
")" { if (lex_debug) printf("%s : CLOSE_PAREN\n", yytext); return ')'; }
"[" { if (lex_debug) printf("%s : OPEN_BRACKET\n", yytext); return '['; }
"]" { if (lex_debug) printf("%s : CLOSE_BRACKET\n", yytext); return ']'; }
"{" { if (lex_debug) printf("%s : OPEN_CURLY\n", yytext); return '{'; }
"}" { if (lex_debug) printf("%s : CLOSE_CURLY\n", yytext); return '}'; }
"," { if (lex_debug) printf("%s : COMMA\n", yytext); return ','; }
";" { if (lex_debug) printf("%s : SEMICOLON\n", yytext); return ';'; }
"=" { if (lex_debug) printf("%s : ASSIGN\n", yytext); return '='; }

"==" { if (lex_debug) printf("%s : EQ\n", yytext); return EQ; }
"!=" { if (lex_debug) printf("%s : NEQ\n", yytext); return NEQ; }
">=" { if (lex_debug) printf("%s : GE\n", yytext); return GE; }
"<=" { if (lex_debug) printf("%s : LE\n", yytext); return LE; }
">" { if (lex_debug) printf("%s : GT\n", yytext); return '>'; }
"<" { if (lex_debug) printf("%s : LT\n", yytext); return '<'; }
"&&" { if (lex_debug) printf("%s : AND\n", yytext); return AND; }
"||" { if (lex_debug) printf("%s : OR\n", yytext); return OR; }
"+" { if (lex_debug) printf("%s : PLUS\n", yytext); return '+'; }
"-" { if (lex_debug) printf("%s : MINUS\n", yytext); return '-'; }
"*" { if (lex_debug) printf("%s : MUL\n", yytext); return '*'; }
"/" { if (lex_debug) printf("%s : DIV\n", yytext); return '/'; }
"++" { if (lex_debug) printf("%s : INC\n", yytext); return INC; }
"--" { if (lex_debug) printf("%s : DEC\n", yytext); return DEC; }
"!" { if (lex_debug) printf("%s : NOT\n", yytext); return '!'; }


[ \t\n] { }


. { /* ignore unknown characters or handle errors quietly */ }

%%

int yywrap() {
    return 1;
}