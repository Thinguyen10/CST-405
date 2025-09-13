#ifndef CODEGEN_H
#define CODEGEN_H

/*
 * Three-Address Code Generation for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include "ast.h"
#include "symtab.h"

/* Three-address code instruction types */
typedef enum {
    /* Arithmetic operations */
    TAC_ADD,        /* x = y + z */
    TAC_SUB,        /* x = y - z */
    TAC_MUL,        /* x = y * z */
    TAC_DIV,        /* x = y / z */
    TAC_NEG,        /* x = -y */
    
    /* Assignment */
    TAC_ASSIGN,     /* x = y */
    TAC_LOAD_CONST, /* x = constant */
    
    /* Array operations */
    TAC_ARRAY_LOAD, /* x = y[z] */
    TAC_ARRAY_STORE,/* x[y] = z */
    
    /* Comparison operations */
    TAC_LT,         /* x = y < z */
    TAC_LTE,        /* x = y <= z */
    TAC_GT,         /* x = y > z */
    TAC_GTE,        /* x = y >= z */
    TAC_EQ,         /* x = y == z */
    TAC_NEQ,        /* x = y != z */
    
    /* Control flow */
    TAC_GOTO,       /* goto L */
    TAC_IF_TRUE,    /* if x goto L */
    TAC_IF_FALSE,   /* if !x goto L */
    TAC_LABEL,      /* L: */
    
    /* Function calls */
    TAC_PARAM,      /* param x */
    TAC_CALL,       /* x = call f, n */
    TAC_RETURN,     /* return x */
    
    /* Function definition */
    TAC_FUNC_BEGIN, /* begin_func f */
    TAC_FUNC_END    /* end_func */
} TACOpcode;

/* Three-address code instruction */
typedef struct TACInstruction {
    TACOpcode opcode;
    char *result;      /* Result operand */
    char *arg1;        /* First argument */
    char *arg2;        /* Second argument */
    int label;         /* Label number (for jumps) */
    struct TACInstruction *next;
} TACInstruction;

/* Three-address code generation context */
typedef struct {
    TACInstruction *head;     /* First instruction */
    TACInstruction *tail;     /* Last instruction */
    int temp_count;           /* Temporary variable counter */
    int label_count;          /* Label counter */
} TACContext;

/* Global TAC context */
extern TACContext *tac_context;

/* TAC generation functions */
void init_tac_generation(void);
void generate_tac(ASTNode *tree);
TACInstruction *gen_tac_node(ASTNode *node);

/* Expression code generation */
char *gen_tac_expression(ASTNode *node);
char *gen_tac_binary_op(ASTNode *node);
char *gen_tac_assignment(ASTNode *node);
char *gen_tac_call(ASTNode *node);
char *gen_tac_var(ASTNode *node);

/* Statement code generation */
void gen_tac_statement(ASTNode *node);
void gen_tac_if(ASTNode *node);
void gen_tac_while(ASTNode *node);
void gen_tac_return(ASTNode *node);
void gen_tac_compound(ASTNode *node);

/* Declaration code generation */
void gen_tac_func_decl(ASTNode *node);
void gen_tac_var_decl(ASTNode *node);

/* TAC instruction creation */
TACInstruction *create_tac(TACOpcode op, char *result, char *arg1, char *arg2);
void emit_tac(TACInstruction *instr);
void emit_label(int label);
void emit_goto(int label);
void emit_conditional(char *cond, int label, int if_true);

/* Temporary variable and label management */
char *new_temp(void);
int new_label(void);
char *make_string(const char *format, ...);

/* TAC output */
void print_tac(void);
void print_tac_instruction(TACInstruction *instr);
void write_tac_to_file(FILE *file);

/* TAC optimization hooks */
TACInstruction *get_tac_list(void);
void set_tac_list(TACInstruction *list);

/* Memory management */
void free_tac(void);

#endif /* CODEGEN_H */