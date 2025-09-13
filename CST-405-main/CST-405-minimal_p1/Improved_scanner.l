%{
/* IMPROVED LEXICAL ANALYZER (SCANNER) WITH PRINTS
* Same format as old scanner, but prints tokens for debugging
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h" /* Token definitions from Bison */
%}

/* Flex options */
%option nounput
%option noinput

/* Regular expression macros */
letter [a-zA-Z]
digit [0-9]
ID {letter}({letter}|{digit})*
TYPE (int|void|float|char|double|long|short)
KEYWORD (if|else|while|for|write|writeln|read|return|break)
NUMBER {digit}+(\.{digit}+)?([eE][+\-]?{digit}+)?

%%

{TYPE} {
    printf("%s : TYPE\n", yytext);
    return TYPE;
}

{KEYWORD} {
    printf("%s : KEYWORD\n", yytext);
    return KEYWORD;
}
"print" {
    printf("%s : PRINT\n", yytext);
    return PRINT;
}

{ID} {
    printf("%s : IDENTIFIER\n", yytext);
    yylval.str = strdup(yytext);
    return ID;
}

{NUMBER} {
    printf("%s : NUMBER\n", yytext);
    yylval.num = atof(yytext);
    return NUM;
}


"(" { printf("%s : OPEN_PAREN\n", yytext); return '('; }
")" { printf("%s : CLOSE_PAREN\n", yytext); return ')'; }
"[" { printf("%s : OPEN_BRACKET\n", yytext); return '['; }
"]" { printf("%s : CLOSE_BRACKET\n", yytext); return ']'; }
"{" { printf("%s : OPEN_CURLY\n", yytext); return '{'; }
"}" { printf("%s : CLOSE_CURLY\n", yytext); return '}'; }
"," { printf("%s : COMMA\n", yytext); return ','; }
";" { printf("%s : SEMICOLON\n", yytext); return ';'; }
"=" { printf("%s : ASSIGN\n", yytext); return '='; }

"==" { printf("%s : EQ\n", yytext); return EQ; }
"!=" { printf("%s : NEQ\n", yytext); return NEQ; }
">=" { printf("%s : GE\n", yytext); return GE; }
"<=" { printf("%s : LE\n", yytext); return LE; }
">" { printf("%s : GT\n", yytext); return '>'; }
"<" { printf("%s : LT\n", yytext); return '<'; }
"&&" { printf("%s : AND\n", yytext); return AND; }
"||" { printf("%s : OR\n", yytext); return OR; }
"+" { printf("%s : PLUS\n", yytext); return '+'; }
"-" { printf("%s : MINUS\n", yytext); return '-'; }
"*" { printf("%s : MUL\n", yytext); return '*'; }
"/" { printf("%s : DIV\n", yytext); return '/'; }
"++" { printf("%s : INC\n", yytext); return INC; }
"--" { printf("%s : DEC\n", yytext); return DEC; }
"!" { printf("%s : NOT\n", yytext); return '!'; }


[ \t\n] { }


. {
    printf("Lexical Error: Unknown character '%c'\n", *yytext);
}

%%

int yywrap() {
    return 1;
}