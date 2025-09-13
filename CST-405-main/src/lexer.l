%{
/* 
 * Lexical Analyzer for C-Minus Language
 * CST-405 Compiler Design
 * 
 * This flex specification defines the tokens for the C-Minus language
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "parser.tab.h"

int linenum = 1;
int colnum = 1;

void count_columns();
void handle_comment();
%}

%option noyywrap

digit       [0-9]
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
number      {digit}+
whitespace  [ \t]+
newline     \n

%%

"/*"            { handle_comment(); }
"if"            { count_columns(); return IF; }
"else"          { count_columns(); return ELSE; }
"int"           { count_columns(); return INT; }
"return"        { count_columns(); return RETURN; }
"void"          { count_columns(); return VOID; }
"while"         { count_columns(); return WHILE; }

"+"             { count_columns(); return PLUS; }
"-"             { count_columns(); return MINUS; }
"*"             { count_columns(); return TIMES; }
"/"             { count_columns(); return DIVIDE; }
"<"             { count_columns(); return LT; }
"<="            { count_columns(); return LTE; }
">"             { count_columns(); return GT; }
">="            { count_columns(); return GTE; }
"=="            { count_columns(); return EQ; }
"!="            { count_columns(); return NEQ; }
"="             { count_columns(); return ASSIGN; }
";"             { count_columns(); return SEMI; }
","             { count_columns(); return COMMA; }
"("             { count_columns(); return LPAREN; }
")"             { count_columns(); return RPAREN; }
"["             { count_columns(); return LBRACKET; }
"]"             { count_columns(); return RBRACKET; }
"{"             { count_columns(); return LBRACE; }
"}"             { count_columns(); return RBRACE; }

{identifier}    { 
                    count_columns(); 
                    yylval.string = strdup(yytext);
                    return ID; 
                }

{number}        { 
                    count_columns(); 
                    yylval.number = atoi(yytext);
                    return NUM; 
                }

{whitespace}    { count_columns(); }

{newline}       { 
                    linenum++; 
                    colnum = 1; 
                }

.               { 
                    fprintf(stderr, "Lexical error at line %d, column %d: unexpected character '%s'\n", 
                            linenum, colnum, yytext);
                    count_columns();
                }

%%

void count_columns() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\t')
            colnum += 8 - (colnum % 8);
        else
            colnum++;
    }
}

void handle_comment() {
    int c;
    int prev = 0;
    
    while ((c = input()) != 0) {
        if (c == '\n') {
            linenum++;
            colnum = 1;
        } else {
            colnum++;
        }
        
        if (prev == '*' && c == '/') {
            break;
        }
        prev = c;
    }
    
    if (c == 0) {
        fprintf(stderr, "Error: Unterminated comment at line %d\n", linenum);
    }
}