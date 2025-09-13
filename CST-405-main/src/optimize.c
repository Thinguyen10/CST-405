/*
 * Optimizer Implementation
 * CST-405 Compiler Design
 * 
 * Demonstrates basic optimization techniques on 3-address code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "optimize.h"
#include "codegen.h"
#include "globals.h"

OptimizationStats opt_stats = {0};

/* Main optimization function */
void optimize_tac(OptimizationLevel level) {
    if (level == OPT_NONE) return;
    
    printf("\n=== OPTIMIZATION PHASE ===\n");
    printf("Optimization level: %d\n", level);
    
    /* Count original instructions */
    TACInstruction *instr = get_tac_list();
    opt_stats.original_instruction_count = 0;
    while (instr) {
        opt_stats.original_instruction_count++;
        instr = instr->next;
    }
    
    /* Basic optimizations */
    constant_folding();
    constant_propagation();
    dead_code_elimination();
    copy_propagation();
    algebraic_simplification();
    
    if (level >= OPT_AGGRESSIVE) {
        /* More aggressive optimizations */
        common_subexpression_elimination();
        peephole_optimization();
    }
    
    /* Count optimized instructions */
    instr = get_tac_list();
    opt_stats.optimized_instruction_count = 0;
    while (instr) {
        opt_stats.optimized_instruction_count++;
        instr = instr->next;
    }
    
    print_optimization_stats();
}

/* Constant folding - evaluate constant expressions at compile time */
void constant_folding(void) {
    TACInstruction *instr = get_tac_list();
    
    while (instr) {
        if (instr->opcode >= TAC_ADD && instr->opcode <= TAC_DIV) {
            if (is_constant(instr->arg1) && is_constant(instr->arg2)) {
                int val1 = get_constant_value(instr->arg1);
                int val2 = get_constant_value(instr->arg2);
                int result = 0;
                
                switch (instr->opcode) {
                    case TAC_ADD: result = val1 + val2; break;
                    case TAC_SUB: result = val1 - val2; break;
                    case TAC_MUL: result = val1 * val2; break;
                    case TAC_DIV: 
                        if (val2 != 0) result = val1 / val2;
                        else continue;  /* Skip division by zero */
                        break;
                    default: continue;
                }
                
                /* Replace with constant load */
                instr->opcode = TAC_LOAD_CONST;
                free(instr->arg1);
                instr->arg1 = make_string("%d", result);
                free(instr->arg2);
                instr->arg2 = NULL;
                
                opt_stats.constants_folded++;
            }
        }
        instr = instr->next;
    }
}

/* Constant propagation - replace variables with known constant values */
void constant_propagation(void) {
    TACInstruction *instr = get_tac_list();
    
    /* Simple constant tracking (local to basic blocks) */
    typedef struct {
        char *var;
        char *value;
    } ConstantEntry;
    
    ConstantEntry constants[100];
    int const_count = 0;
    
    while (instr) {
        /* Check if this is a constant assignment */
        if (instr->opcode == TAC_LOAD_CONST) {
            /* Record constant */
            int found = 0;
            for (int i = 0; i < const_count; i++) {
                if (strcmp(constants[i].var, instr->result) == 0) {
                    free(constants[i].value);
                    constants[i].value = copy_string(instr->arg1);
                    found = 1;
                    break;
                }
            }
            if (!found && const_count < 100) {
                constants[const_count].var = copy_string(instr->result);
                constants[const_count].value = copy_string(instr->arg1);
                const_count++;
            }
        }
        /* Replace uses of constants */
        else if (instr->opcode != TAC_FUNC_BEGIN && instr->opcode != TAC_FUNC_END) {
            for (int i = 0; i < const_count; i++) {
                if (instr->arg1 && strcmp(instr->arg1, constants[i].var) == 0) {
                    free(instr->arg1);
                    instr->arg1 = copy_string(constants[i].value);
                    opt_stats.constants_folded++;
                }
                if (instr->arg2 && strcmp(instr->arg2, constants[i].var) == 0) {
                    free(instr->arg2);
                    instr->arg2 = copy_string(constants[i].value);
                    opt_stats.constants_folded++;
                }
            }
        }
        
        /* Clear constants at labels (conservative) */
        if (instr->opcode == TAC_LABEL) {
            const_count = 0;
        }
        
        instr = instr->next;
    }
}

/* Dead code elimination - remove code that doesn't affect output */
void dead_code_elimination(void) {
    TACInstruction *instr = get_tac_list();
    TACInstruction *prev = NULL;
    
    while (instr) {
        TACInstruction *next = instr->next;
        int is_dead = 0;
        
        /* Check if instruction result is never used */
        if (instr->result && is_temporary(instr->result)) {
            /* Simple check: is this temp used in any following instruction? */
            TACInstruction *check = instr->next;
            int used = 0;
            
            while (check && !used) {
                if ((check->arg1 && strcmp(check->arg1, instr->result) == 0) ||
                    (check->arg2 && strcmp(check->arg2, instr->result) == 0) ||
                    (check->opcode == TAC_PARAM && check->result && 
                     strcmp(check->result, instr->result) == 0)) {
                    used = 1;
                }
                check = check->next;
            }
            
            if (!used && instr->opcode != TAC_CALL) {
                is_dead = 1;
            }
        }
        
        if (is_dead) {
            /* Remove instruction */
            if (prev) {
                prev->next = next;
            } else {
                set_tac_list(next);
            }
            free(instr);
            opt_stats.dead_code_removed++;
        } else {
            prev = instr;
        }
        
        instr = next;
    }
}

/* Copy propagation - replace copies with original values */
void copy_propagation(void) {
    TACInstruction *instr = get_tac_list();
    
    typedef struct {
        char *dest;
        char *source;
    } CopyEntry;
    
    CopyEntry copies[100];
    int copy_count = 0;
    
    while (instr) {
        /* Check if this is a copy: x = y */
        if (instr->opcode == TAC_ASSIGN) {
            /* Record copy */
            int found = 0;
            for (int i = 0; i < copy_count; i++) {
                if (strcmp(copies[i].dest, instr->result) == 0) {
                    free(copies[i].source);
                    copies[i].source = copy_string(instr->arg1);
                    found = 1;
                    break;
                }
            }
            if (!found && copy_count < 100) {
                copies[copy_count].dest = copy_string(instr->result);
                copies[copy_count].source = copy_string(instr->arg1);
                copy_count++;
            }
        }
        /* Replace uses of copies */
        else if (instr->opcode != TAC_FUNC_BEGIN && instr->opcode != TAC_FUNC_END) {
            for (int i = 0; i < copy_count; i++) {
                if (instr->arg1 && strcmp(instr->arg1, copies[i].dest) == 0) {
                    free(instr->arg1);
                    instr->arg1 = copy_string(copies[i].source);
                    opt_stats.copies_propagated++;
                }
                if (instr->arg2 && strcmp(instr->arg2, copies[i].dest) == 0) {
                    free(instr->arg2);
                    instr->arg2 = copy_string(copies[i].source);
                    opt_stats.copies_propagated++;
                }
            }
        }
        
        /* Clear copies at labels (conservative) */
        if (instr->opcode == TAC_LABEL) {
            copy_count = 0;
        }
        
        instr = instr->next;
    }
}

/* Algebraic simplification - simplify algebraic expressions */
void algebraic_simplification(void) {
    TACInstruction *instr = get_tac_list();
    
    while (instr) {
        /* x = y + 0  =>  x = y */
        if (instr->opcode == TAC_ADD) {
            if (is_constant(instr->arg2) && get_constant_value(instr->arg2) == 0) {
                instr->opcode = TAC_ASSIGN;
                free(instr->arg2);
                instr->arg2 = NULL;
                opt_stats.expressions_simplified++;
            } else if (is_constant(instr->arg1) && get_constant_value(instr->arg1) == 0) {
                instr->opcode = TAC_ASSIGN;
                free(instr->arg1);
                instr->arg1 = instr->arg2;
                instr->arg2 = NULL;
                opt_stats.expressions_simplified++;
            }
        }
        
        /* x = y * 1  =>  x = y */
        if (instr->opcode == TAC_MUL) {
            if (is_constant(instr->arg2) && get_constant_value(instr->arg2) == 1) {
                instr->opcode = TAC_ASSIGN;
                free(instr->arg2);
                instr->arg2 = NULL;
                opt_stats.expressions_simplified++;
            } else if (is_constant(instr->arg1) && get_constant_value(instr->arg1) == 1) {
                instr->opcode = TAC_ASSIGN;
                free(instr->arg1);
                instr->arg1 = instr->arg2;
                instr->arg2 = NULL;
                opt_stats.expressions_simplified++;
            }
        }
        
        /* x = y * 0  =>  x = 0 */
        if (instr->opcode == TAC_MUL) {
            if ((is_constant(instr->arg1) && get_constant_value(instr->arg1) == 0) ||
                (is_constant(instr->arg2) && get_constant_value(instr->arg2) == 0)) {
                instr->opcode = TAC_LOAD_CONST;
                free(instr->arg1);
                instr->arg1 = copy_string("0");
                free(instr->arg2);
                instr->arg2 = NULL;
                opt_stats.expressions_simplified++;
            }
        }
        
        instr = instr->next;
    }
}

/* Common subexpression elimination */
void common_subexpression_elimination(void) {
    TACInstruction *instr = get_tac_list();
    
    typedef struct {
        TACOpcode op;
        char *arg1;
        char *arg2;
        char *result;
    } ExprEntry;
    
    ExprEntry expressions[100];
    int expr_count = 0;
    
    while (instr) {
        if (instr->opcode >= TAC_ADD && instr->opcode <= TAC_NEQ) {
            /* Check if expression already computed */
            int found = -1;
            for (int i = 0; i < expr_count; i++) {
                if (expressions[i].op == instr->opcode &&
                    strcmp(expressions[i].arg1, instr->arg1) == 0 &&
                    strcmp(expressions[i].arg2, instr->arg2) == 0) {
                    found = i;
                    break;
                }
            }
            
            if (found >= 0) {
                /* Replace with copy of previous result */
                instr->opcode = TAC_ASSIGN;
                free(instr->arg1);
                instr->arg1 = copy_string(expressions[found].result);
                free(instr->arg2);
                instr->arg2 = NULL;
                opt_stats.subexpressions_eliminated++;
            } else if (expr_count < 100) {
                /* Record new expression */
                expressions[expr_count].op = instr->opcode;
                expressions[expr_count].arg1 = copy_string(instr->arg1);
                expressions[expr_count].arg2 = copy_string(instr->arg2);
                expressions[expr_count].result = copy_string(instr->result);
                expr_count++;
            }
        }
        
        /* Clear expressions at labels (conservative) */
        if (instr->opcode == TAC_LABEL) {
            expr_count = 0;
        }
        
        instr = instr->next;
    }
}

/* Peephole optimization - optimize small instruction sequences */
void peephole_optimization(void) {
    remove_redundant_jumps();
    combine_operations();
}

/* Remove redundant jumps */
void remove_redundant_jumps(void) {
    TACInstruction *instr = get_tac_list();
    
    while (instr && instr->next) {
        /* Remove jump to next instruction */
        if (instr->opcode == TAC_GOTO && 
            instr->next->opcode == TAC_LABEL &&
            instr->label == instr->next->label) {
            /* Remove the goto */
            TACInstruction *to_remove = instr;
            instr = instr->next;
            
            /* Fix linked list */
            TACInstruction *prev = get_tac_list();
            if (prev == to_remove) {
                set_tac_list(instr);
            } else {
                while (prev->next != to_remove) {
                    prev = prev->next;
                }
                prev->next = instr;
            }
            
            free(to_remove);
            opt_stats.dead_code_removed++;
        } else {
            instr = instr->next;
        }
    }
}

/* Combine operations */
void combine_operations(void) {
    /* Example: combine consecutive adds/multiplies */
    /* This is a placeholder for more complex operation combining */
}

/* Check if operand is a constant */
int is_constant(char *operand) {
    if (!operand) return 0;
    
    /* Check if it's a number (possibly negative) */
    char *p = operand;
    if (*p == '-') p++;
    
    while (*p) {
        if (!isdigit(*p)) return 0;
        p++;
    }
    
    return 1;
}

/* Get constant value */
int get_constant_value(char *operand) {
    return atoi(operand);
}

/* Check if operand is a temporary */
int is_temporary(char *operand) {
    return operand && operand[0] == 't' && isdigit(operand[1]);
}

/* Print optimization statistics */
void print_optimization_stats(void) {
    printf("\n=== OPTIMIZATION STATISTICS ===\n");
    printf("Original instructions:     %d\n", opt_stats.original_instruction_count);
    printf("Optimized instructions:    %d\n", opt_stats.optimized_instruction_count);
    printf("Instructions removed:      %d\n", 
           opt_stats.original_instruction_count - opt_stats.optimized_instruction_count);
    printf("Constants folded:          %d\n", opt_stats.constants_folded);
    printf("Dead code removed:         %d\n", opt_stats.dead_code_removed);
    printf("Copies propagated:         %d\n", opt_stats.copies_propagated);
    printf("Expressions simplified:    %d\n", opt_stats.expressions_simplified);
    printf("Subexpressions eliminated: %d\n", opt_stats.subexpressions_eliminated);
    
    if (opt_stats.original_instruction_count > 0) {
        float reduction = 100.0 * (opt_stats.original_instruction_count - 
                                   opt_stats.optimized_instruction_count) /
                         opt_stats.original_instruction_count;
        printf("Code size reduction:       %.1f%%\n", reduction);
    }
}