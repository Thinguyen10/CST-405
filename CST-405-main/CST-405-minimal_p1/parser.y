
/* a script built from bison by make command */
%{
/* SYNTAX ANALYZER (PARSER)
 * This is the second phase of compilation - checking grammar rules
 * Bison generates a parser that builds an Abstract Syntax Tree (AST)
 * The parser uses tokens from the scanner to verify syntax is correct
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* External declarations for lexer interface */
extern int yylex();      /* Get next token from scanner */
extern int yyparse();    /* Parse the entire input */
extern FILE* yyin;       /* Input file handle */

void yyerror(const char* s);  /* Error handling function */
ASTNode* root = NULL;          /* Root of the Abstract Syntax Tree */
%}

/* SEMANTIC VALUES UNION
 * Defines possible types for tokens and grammar symbols
 * This allows different grammar rules to return different data types
 */
%union {
    double num;                /* Thi added - For numeric (float) literals */
    char* str;              /* For identifiers */
    struct ASTNode* node;   /* For AST nodes (statements, expressions, types, etc.) */
}

/* TOKEN DECLARATIONS with their semantic value types */
%token <num> NUM  /* numeric literals */
%token <str> ID
%token PRINT TYPE KEYWORD     /* Thi added all */
%token IF ELSE WHILE FOR RETURN BREAK
%token EQ NEQ GE LE AND OR INC DEC  /* Thi added */


/* Nonterminal semantic types */
%type <node> program stmt_list stmt decl assign expr  assign_expr print_stmt
%type <node> if_stmt while_stmt for_stmt return_stmt block expr_list
%type <node> for_init for_cond for_update


/* DEFINE OPERATOR PRECEDENCE 
Ex: * over + -
*/

%left OR
%left AND
%nonassoc EQ NEQ
%nonassoc '>' '<' GE LE
%left '+' '-'
%left '*' '/'
%right UMINUS     /* unary minus */

/* start symbol */
%start program

%% 

/* GRAMMAR RULES - Define the structure of our language */
/* Goes from program -> stmt list -> stmt -> decl -> assign -> expr */

/* PROGRAM RULE - Entry point of our grammar*/
program:
    stmt_list { 
        /* Action: Save the statement list as our AST root */
        root = $1;  /* $1 refers to the first symbol (stmt_list) */
    }
    ;

/* STATEMENT LIST - could be expanded to stmt | stmt_list stmt */
stmt_list:
    stmt { 
        /* Base case: single statement */
        $$ = $1;  /* Pass the statement up as-is */
    }
    | stmt_list stmt { 
        /* Recursive case: list followed by another statement */
        $$ = createStmtList($1, $2);  /* Build linked list of statements */
    }
    ;

/* STATEMENT TYPES - means a stmt can be either a decl, assign, print_stmt, or others */
stmt:
    decl        /* Variable declaration */
    | assign    /* Assignment statement */
    | print_stmt /* Print statement */
    | if_stmt     /* Thi added */
    | while_stmt
    | for_stmt
    | return_stmt
    | BREAK ';' { $$ = createBreak(); }
    ;

/* BLOCK: sequence of statements inside curly braces */
block:
    '{' stmt_list '}' { $$ = $2; }
    ;

/* DECLARATION RULE - "int x;" */
decl:
    TYPE ID ';' { 
        /* Create declaration node and free the identifier string */
        $$ = createDecl($2, NULL);  /* $2 is token #2, meaning take ID */
        free($2);             /* And free the string copy ID from scanner */
    }
    |
    TYPE ID '=' expr ';' { /* Thi added */
        $$ = createDecl($2, $4);  
        free($2);    
    }
    ;

/* Assignment expression (NO ;) for for-loops */
assign_expr:
    ID '=' expr {
        $$ = createAssign($1, $3);  /* Reuse the same AST constructor */
        free($1);
    }
    ;

/* ASSIGNMENT RULE - "x = expr;" */
assign:
    ID '=' expr ';' { 
        /* Create assignment node with variable name and expression */
        $$ = createAssign($1, $3);  /* $1 = ID, $3 = expr */
        free($1);                   /* Free the identifier string */
    }
    ;

/* EXPRESSION RULES - Build expression trees */
expr:
    NUM { 
        /* Literal number */
        $$ = createNum($1);  /* $1 is of type int */
    }
    | ID { 
        /* Variable reference */
        $$ = createVar($1);  /* $1 is char*/
        free($1);            /* Free the identifier string */
    }
    | expr '+' expr { $$ = createBinOp("+", $1, $3);  }
    | expr '-' expr { $$ = createBinOp("-", $1, $3);  } /* Thi added all */
    | expr '*' expr { $$ = createBinOp("*", $1, $3);  }
    | expr '/' expr { $$ = createBinOp("/", $1, $3);  }
    | expr EQ expr  { $$ = createBinOp("==", $1, $3); }  /* EQ from lexer ("==") */
    | expr NEQ expr { $$ = createBinOp("!=", $1, $3); }  /* NEQ from lexer ("!=") */
    | expr GE expr  { $$ = createBinOp(">=", $1, $3); }  /* GE from lexer (">=") */
    | expr LE expr  { $$ = createBinOp("<=", $1, $3); }  /* LE from lexer ("<=") */
    | expr '>' expr { $$ = createBinOp(">", $1, $3); }   /* '>' returned as '>' */
    | expr '<' expr { $$ = createBinOp("<", $1, $3); }   /* '<' returned as '<' */
    | expr AND expr { $$ = createBinOp("&&", $1, $3); }  /* AND token ("&&") */
    | expr OR expr  { $$ = createBinOp("||", $1, $3); }  /* OR token ("||") */
    | '-' expr %prec UMINUS { $$ = createUnaryOp("-", $2); }
    | '(' expr ')' { $$ = $2; }
    ;

/* EXPRESSION LIST RULES */
expr_list:
    expr {
        $$ = createExprList($1, NULL);
    }
    |
    expr_list ',' expr {
        $$ = addToExprList($1, $3);
    }


/* PRINT STATEMENT - "print(expr);" */
print_stmt:
    PRINT '(' expr_list ')' ';' { 
        /* Create print node with expression to print */
        $$ = createPrint($3);  /* $3 is the expression inside parens */
    }
    ;


/* IF statement: supports optional ELSE
   NOTE: lexer must return distinct tokens for 'if' and 'else' for this to work.
   See lexer notes below. */
if_stmt:
    /* if (cond) block */
    IF '(' expr ')' block {
        $$ = createIf($3, $5, NULL);
    }
    | IF '(' expr ')' block ELSE block {
        $$ = createIf($3, $5, $7);
    }
    ;

/* WHILE statement */
while_stmt:
    WHILE '(' expr ')' block {
        $$ = createWhile($3, $5);
    }
    ;

/* FOR statement: simple C-style for(init; cond; update) { body } */
for_stmt:
    FOR '(' for_init ';' for_cond ';' for_update ')' block {
        $$ = createFor($3, $5, $7, $9);
    }
    ;

for_init:
    decl
    | assign_expr
    | /* empty since ; already included */   { $$ = NULL; } 
    ;

for_cond:
    expr    { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

for_update:
    assign_expr    { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;


/* RETURN statement */
return_stmt:
    RETURN expr ';' {
        $$ = createReturn($2);
    }
    ;

%%

/* ERROR HANDLING - Called by Bison when syntax error detected */
void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error: %s\n", s);
}