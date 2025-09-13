%{
/*
 * Parser for C-Minus Language
 * CST-405 Compiler Design
 * 
 * This bison specification defines the grammar rules and builds the AST
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "ast.h"
#include "symtab.h"

extern int yylex();
extern int linenum;
extern int colnum;
extern FILE *yyin;

void yyerror(const char *s);

ASTNode *ast_root = NULL;
%}

%union {
    int number;
    char *string;
    struct ASTNode *node;
}

/* Token declarations */
%token <number> NUM
%token <string> ID
%token IF ELSE INT RETURN VOID WHILE
%token PLUS MINUS TIMES DIVIDE
%token LT LTE GT GTE EQ NEQ
%token ASSIGN SEMI COMMA
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE

/* Non-terminal type declarations */
%type <node> program declaration_list declaration var_declaration
%type <node> fun_declaration params param_list param
%type <node> compound_stmt local_declarations statement_list
%type <node> statement expression_stmt selection_stmt iteration_stmt
%type <node> return_stmt expression var simple_expression
%type <node> additive_expression term factor call args arg_list
%type <string> type_specifier relop addop mulop

/* Precedence and associativity */
%right ASSIGN
%left EQ NEQ
%left LT LTE GT GTE
%left PLUS MINUS
%left TIMES DIVIDE

%%

program:
    declaration_list { 
        ast_root = $1; 
        printf("Parse completed successfully!\n");
    }
    ;

declaration_list:
    declaration_list declaration { 
        $$ = create_node(NODE_PROGRAM);
        $$->left = $1;
        $$->right = $2;
    }
    | declaration { 
        $$ = $1; 
    }
    ;

declaration:
    var_declaration { $$ = $1; }
    | fun_declaration { $$ = $1; }
    ;

var_declaration:
    type_specifier ID SEMI { 
        $$ = create_var_decl_node($1, $2, 0);
    }
    | type_specifier ID LBRACKET NUM RBRACKET SEMI { 
        $$ = create_var_decl_node($1, $2, $4);
    }
    ;

type_specifier:
    INT { $$ = strdup("int"); }
    | VOID { $$ = strdup("void"); }
    ;

fun_declaration:
    type_specifier ID LPAREN params RPAREN compound_stmt {
        $$ = create_func_decl_node($1, $2, $4, $6);
    }
    ;

params:
    param_list { $$ = $1; }
    | VOID { $$ = NULL; }
    ;

param_list:
    param_list COMMA param {
        $$ = create_node(NODE_PARAM_LIST);
        $$->left = $1;
        $$->right = $3;
    }
    | param { $$ = $1; }
    ;

param:
    type_specifier ID {
        $$ = create_param_node($1, $2, 0);
    }
    | type_specifier ID LBRACKET RBRACKET {
        $$ = create_param_node($1, $2, 1);
    }
    ;

compound_stmt:
    LBRACE local_declarations statement_list RBRACE {
        $$ = create_compound_stmt_node($2, $3);
    }
    ;

local_declarations:
    local_declarations var_declaration {
        $$ = create_node(NODE_LOCAL_DECLS);
        $$->left = $1;
        $$->right = $2;
    }
    | /* empty */ { $$ = NULL; }
    ;

statement_list:
    statement_list statement {
        $$ = create_node(NODE_STMT_LIST);
        $$->left = $1;
        $$->right = $2;
    }
    | /* empty */ { $$ = NULL; }
    ;

statement:
    expression_stmt { $$ = $1; }
    | compound_stmt { $$ = $1; }
    | selection_stmt { $$ = $1; }
    | iteration_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    ;

expression_stmt:
    expression SEMI { $$ = $1; }
    | SEMI { $$ = NULL; }
    ;

selection_stmt:
    IF LPAREN expression RPAREN statement {
        $$ = create_if_node($3, $5, NULL);
    }
    | IF LPAREN expression RPAREN statement ELSE statement {
        $$ = create_if_node($3, $5, $7);
    }
    ;

iteration_stmt:
    WHILE LPAREN expression RPAREN statement {
        $$ = create_while_node($3, $5);
    }
    ;

return_stmt:
    RETURN SEMI {
        $$ = create_return_node(NULL);
    }
    | RETURN expression SEMI {
        $$ = create_return_node($2);
    }
    ;

expression:
    var ASSIGN expression {
        $$ = create_assign_node($1, $3);
    }
    | simple_expression { $$ = $1; }
    ;

var:
    ID {
        $$ = create_id_node($1);
    }
    | ID LBRACKET expression RBRACKET {
        $$ = create_array_access_node($1, $3);
    }
    ;

simple_expression:
    additive_expression relop additive_expression {
        $$ = create_binary_op_node($2, $1, $3);
    }
    | additive_expression { $$ = $1; }
    ;

relop:
    LTE { $$ = strdup("<="); }
    | LT { $$ = strdup("<"); }
    | GT { $$ = strdup(">"); }
    | GTE { $$ = strdup(">="); }
    | EQ { $$ = strdup("=="); }
    | NEQ { $$ = strdup("!="); }
    ;

additive_expression:
    additive_expression addop term {
        $$ = create_binary_op_node($2, $1, $3);
    }
    | term { $$ = $1; }
    ;

addop:
    PLUS { $$ = strdup("+"); }
    | MINUS { $$ = strdup("-"); }
    ;

term:
    term mulop factor {
        $$ = create_binary_op_node($2, $1, $3);
    }
    | factor { $$ = $1; }
    ;

mulop:
    TIMES { $$ = strdup("*"); }
    | DIVIDE { $$ = strdup("/"); }
    ;

factor:
    LPAREN expression RPAREN { $$ = $2; }
    | var { $$ = $1; }
    | call { $$ = $1; }
    | NUM { 
        $$ = create_num_node($1); 
    }
    ;

call:
    ID LPAREN args RPAREN {
        $$ = create_call_node($1, $3);
    }
    ;

args:
    arg_list { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

arg_list:
    arg_list COMMA expression {
        $$ = create_node(NODE_ARG_LIST);
        $$->left = $1;
        $$->right = $3;
    }
    | expression { $$ = $1; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", linenum, colnum, s);
}