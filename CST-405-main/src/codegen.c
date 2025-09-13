/*
 * Three-Address Code Generation Implementation
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "codegen.h"
#include "ast.h"
#include "symtab.h"
#include "globals.h"

/* Global TAC context */
TACContext *tac_context = NULL;

/* Initialize TAC generation */
void init_tac_generation(void) {
    tac_context = (TACContext *)malloc(sizeof(TACContext));
    tac_context->head = NULL;
    tac_context->tail = NULL;
    tac_context->temp_count = 0;
    tac_context->label_count = 0;
}

/* Generate TAC for entire AST */
void generate_tac(ASTNode *tree) {
    if (tree == NULL) return;
    
    printf("\n=== THREE-ADDRESS CODE GENERATION ===\n");
    
    init_tac_generation();
    gen_tac_node(tree);
    
    print_tac();
}

/* Generate TAC for a node */
TACInstruction *gen_tac_node(ASTNode *node) {
    if (node == NULL) return NULL;
    
    switch (node->node_type) {
        case NODE_PROGRAM:
            gen_tac_node(node->left);
            gen_tac_node(node->right);
            break;
            
        case NODE_FUNC_DECL:
            gen_tac_func_decl(node);
            break;
            
        case NODE_VAR_DECL:
            gen_tac_var_decl(node);
            break;
            
        case NODE_COMPOUND_STMT:
            gen_tac_compound(node);
            break;
            
        case NODE_IF_STMT:
            gen_tac_if(node);
            break;
            
        case NODE_WHILE_STMT:
            gen_tac_while(node);
            break;
            
        case NODE_RETURN_STMT:
            gen_tac_return(node);
            break;
            
        case NODE_EXPR_STMT:
            if (node->left) {
                gen_tac_expression(node->left);
            }
            break;
            
        default:
            gen_tac_node(node->left);
            gen_tac_node(node->right);
            gen_tac_node(node->next);
            break;
    }
    
    return NULL;
}

/* Generate TAC for function declaration */
void gen_tac_func_decl(ASTNode *node) {
    char *func_name = node->value.func_decl.name;
    
    /* Emit function begin */
    emit_tac(create_tac(TAC_FUNC_BEGIN, func_name, NULL, NULL));
    
    /* Generate code for function body */
    gen_tac_node(node->right);
    
    /* Emit function end */
    emit_tac(create_tac(TAC_FUNC_END, func_name, NULL, NULL));
}

/* Generate TAC for variable declaration */
void gen_tac_var_decl(ASTNode *node) {
    /* Variable declarations don't generate code in basic TAC */
    /* Memory allocation is handled by the symbol table */
}

/* Generate TAC for compound statement */
void gen_tac_compound(ASTNode *node) {
    /* Generate code for local declarations */
    gen_tac_node(node->left);
    
    /* Generate code for statements */
    gen_tac_node(node->right);
}

/* Generate TAC for expression */
char *gen_tac_expression(ASTNode *node) {
    if (node == NULL) return NULL;
    
    switch (node->node_type) {
        case NODE_NUM:
            {
                char *temp = new_temp();
                char *value = make_string("%d", node->value.int_val);
                emit_tac(create_tac(TAC_LOAD_CONST, temp, value, NULL));
                return temp;
            }
            
        case NODE_ID:
            return gen_tac_var(node);
            
        case NODE_ARRAY_ACCESS:
            return gen_tac_var(node);
            
        case NODE_ASSIGN:
            return gen_tac_assignment(node);
            
        case NODE_BINARY_OP:
            return gen_tac_binary_op(node);
            
        case NODE_CALL:
            return gen_tac_call(node);
            
        default:
            return NULL;
    }
}

/* Generate TAC for binary operation */
char *gen_tac_binary_op(ASTNode *node) {
    char *left = gen_tac_expression(node->left);
    char *right = gen_tac_expression(node->right);
    char *result = new_temp();
    char *op = node->value.string_val;
    
    TACOpcode opcode;
    if (strcmp(op, "+") == 0) opcode = TAC_ADD;
    else if (strcmp(op, "-") == 0) opcode = TAC_SUB;
    else if (strcmp(op, "*") == 0) opcode = TAC_MUL;
    else if (strcmp(op, "/") == 0) opcode = TAC_DIV;
    else if (strcmp(op, "<") == 0) opcode = TAC_LT;
    else if (strcmp(op, "<=") == 0) opcode = TAC_LTE;
    else if (strcmp(op, ">") == 0) opcode = TAC_GT;
    else if (strcmp(op, ">=") == 0) opcode = TAC_GTE;
    else if (strcmp(op, "==") == 0) opcode = TAC_EQ;
    else if (strcmp(op, "!=") == 0) opcode = TAC_NEQ;
    else opcode = TAC_ADD;  /* Default */
    
    emit_tac(create_tac(opcode, result, left, right));
    return result;
}

/* Generate TAC for assignment */
char *gen_tac_assignment(ASTNode *node) {
    char *value = gen_tac_expression(node->right);
    
    if (node->left->node_type == NODE_ARRAY_ACCESS) {
        /* Array assignment: a[i] = value */
        char *array = node->left->value.string_val;
        char *index = gen_tac_expression(node->left->left);
        emit_tac(create_tac(TAC_ARRAY_STORE, array, index, value));
    } else {
        /* Simple assignment: x = value */
        char *var = node->left->value.string_val;
        emit_tac(create_tac(TAC_ASSIGN, var, value, NULL));
    }
    
    return value;
}

/* Generate TAC for variable access */
char *gen_tac_var(ASTNode *node) {
    if (node->node_type == NODE_ARRAY_ACCESS) {
        /* Array access: t = a[i] */
        char *array = node->value.string_val;
        char *index = gen_tac_expression(node->left);
        char *temp = new_temp();
        emit_tac(create_tac(TAC_ARRAY_LOAD, temp, array, index));
        return temp;
    } else {
        /* Simple variable */
        return copy_string(node->value.string_val);
    }
}

/* Generate TAC for function call */
char *gen_tac_call(ASTNode *node) {
    char *func_name = node->value.string_val;
    
    /* Generate code for arguments */
    ASTNode *arg = node->left;
    int arg_count = 0;
    while (arg) {
        char *arg_value;
        if (arg->node_type == NODE_ARG_LIST) {
            arg_value = gen_tac_expression(arg->right);
            arg = arg->left;
        } else {
            arg_value = gen_tac_expression(arg);
            arg = NULL;
        }
        emit_tac(create_tac(TAC_PARAM, arg_value, NULL, NULL));
        arg_count++;
    }
    
    /* Generate call instruction */
    char *result = NULL;
    if (strcmp(func_name, "output") != 0) {
        result = new_temp();
    }
    
    char *count_str = make_string("%d", arg_count);
    emit_tac(create_tac(TAC_CALL, result, func_name, count_str));
    
    return result;
}

/* Generate TAC for if statement */
void gen_tac_if(ASTNode *node) {
    char *cond = gen_tac_expression(node->left);
    int false_label = new_label();
    int end_label = new_label();
    
    /* If condition is false, jump to false_label */
    emit_conditional(cond, false_label, 0);
    
    /* Generate code for then branch */
    gen_tac_node(node->right);
    
    if (node->next) {
        /* If there's an else branch */
        emit_goto(end_label);
        emit_label(false_label);
        gen_tac_node(node->next);
        emit_label(end_label);
    } else {
        emit_label(false_label);
    }
}

/* Generate TAC for while statement */
void gen_tac_while(ASTNode *node) {
    int begin_label = new_label();
    int end_label = new_label();
    
    /* Begin of loop */
    emit_label(begin_label);
    
    /* Evaluate condition */
    char *cond = gen_tac_expression(node->left);
    
    /* If condition is false, exit loop */
    emit_conditional(cond, end_label, 0);
    
    /* Generate code for loop body */
    gen_tac_node(node->right);
    
    /* Jump back to beginning */
    emit_goto(begin_label);
    
    /* End of loop */
    emit_label(end_label);
}

/* Generate TAC for return statement */
void gen_tac_return(ASTNode *node) {
    if (node->left) {
        char *value = gen_tac_expression(node->left);
        emit_tac(create_tac(TAC_RETURN, value, NULL, NULL));
    } else {
        emit_tac(create_tac(TAC_RETURN, NULL, NULL, NULL));
    }
}

/* Create a TAC instruction */
TACInstruction *create_tac(TACOpcode op, char *result, char *arg1, char *arg2) {
    TACInstruction *instr = (TACInstruction *)malloc(sizeof(TACInstruction));
    instr->opcode = op;
    instr->result = result ? copy_string(result) : NULL;
    instr->arg1 = arg1 ? copy_string(arg1) : NULL;
    instr->arg2 = arg2 ? copy_string(arg2) : NULL;
    instr->label = -1;
    instr->next = NULL;
    return instr;
}

/* Emit a TAC instruction */
void emit_tac(TACInstruction *instr) {
    if (tac_context->head == NULL) {
        tac_context->head = instr;
        tac_context->tail = instr;
    } else {
        tac_context->tail->next = instr;
        tac_context->tail = instr;
    }
}

/* Emit a label */
void emit_label(int label) {
    TACInstruction *instr = create_tac(TAC_LABEL, NULL, NULL, NULL);
    instr->label = label;
    emit_tac(instr);
}

/* Emit a goto */
void emit_goto(int label) {
    TACInstruction *instr = create_tac(TAC_GOTO, NULL, NULL, NULL);
    instr->label = label;
    emit_tac(instr);
}

/* Emit a conditional jump */
void emit_conditional(char *cond, int label, int if_true) {
    TACInstruction *instr = create_tac(
        if_true ? TAC_IF_TRUE : TAC_IF_FALSE,
        cond, NULL, NULL
    );
    instr->label = label;
    emit_tac(instr);
}

/* Generate new temporary variable */
char *new_temp(void) {
    return make_string("t%d", tac_context->temp_count++);
}

/* Generate new label */
int new_label(void) {
    return tac_context->label_count++;
}

/* Create a formatted string */
char *make_string(const char *format, ...) {
    va_list args;
    char buffer[256];
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    return copy_string(buffer);
}

/* Print TAC instruction */
void print_tac_instruction(TACInstruction *instr) {
    switch (instr->opcode) {
        case TAC_ADD:
            printf("    %s = %s + %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_SUB:
            printf("    %s = %s - %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_MUL:
            printf("    %s = %s * %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_DIV:
            printf("    %s = %s / %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_ASSIGN:
            printf("    %s = %s\n", instr->result, instr->arg1);
            break;
        case TAC_LOAD_CONST:
            printf("    %s = %s\n", instr->result, instr->arg1);
            break;
        case TAC_ARRAY_LOAD:
            printf("    %s = %s[%s]\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_ARRAY_STORE:
            printf("    %s[%s] = %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_LT:
            printf("    %s = %s < %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_LTE:
            printf("    %s = %s <= %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_GT:
            printf("    %s = %s > %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_GTE:
            printf("    %s = %s >= %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_EQ:
            printf("    %s = %s == %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_NEQ:
            printf("    %s = %s != %s\n", instr->result, instr->arg1, instr->arg2);
            break;
        case TAC_GOTO:
            printf("    goto L%d\n", instr->label);
            break;
        case TAC_IF_TRUE:
            printf("    if %s goto L%d\n", instr->result, instr->label);
            break;
        case TAC_IF_FALSE:
            printf("    if !%s goto L%d\n", instr->result, instr->label);
            break;
        case TAC_LABEL:
            printf("L%d:\n", instr->label);
            break;
        case TAC_PARAM:
            printf("    param %s\n", instr->result);
            break;
        case TAC_CALL:
            if (instr->result) {
                printf("    %s = call %s, %s\n", instr->result, instr->arg1, instr->arg2);
            } else {
                printf("    call %s, %s\n", instr->arg1, instr->arg2);
            }
            break;
        case TAC_RETURN:
            if (instr->result) {
                printf("    return %s\n", instr->result);
            } else {
                printf("    return\n");
            }
            break;
        case TAC_FUNC_BEGIN:
            printf("\nBEGIN_FUNC %s\n", instr->result);
            break;
        case TAC_FUNC_END:
            printf("END_FUNC %s\n\n", instr->result);
            break;
        default:
            printf("    UNKNOWN\n");
    }
}

/* Print all TAC instructions */
void print_tac(void) {
    printf("\n=== THREE-ADDRESS CODE ===\n");
    TACInstruction *instr = tac_context->head;
    while (instr) {
        print_tac_instruction(instr);
        instr = instr->next;
    }
}

/* Get TAC list (for optimizer) */
TACInstruction *get_tac_list(void) {
    return tac_context->head;
}

/* Set TAC list (after optimization) */
void set_tac_list(TACInstruction *list) {
    tac_context->head = list;
    
    /* Find new tail */
    TACInstruction *tail = list;
    while (tail && tail->next) {
        tail = tail->next;
    }
    tac_context->tail = tail;
}