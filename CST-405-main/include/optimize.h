#ifndef OPTIMIZE_H
#define OPTIMIZE_H

/*
 * Optimizer for Three-Address Code
 * CST-405 Compiler Design
 */

#include "codegen.h"

/* Optimization levels */
typedef enum {
    OPT_NONE = 0,
    OPT_BASIC = 1,      /* Basic optimizations */
    OPT_AGGRESSIVE = 2  /* Aggressive optimizations */
} OptimizationLevel;

/* Basic block structure for optimization */
typedef struct BasicBlock {
    int id;
    TACInstruction *start;
    TACInstruction *end;
    struct BasicBlock **predecessors;
    struct BasicBlock **successors;
    int pred_count;
    int succ_count;
    
    /* Data flow information */
    int *live_in;
    int *live_out;
    int *gen;
    int *kill;
} BasicBlock;

/* Optimization passes */
void optimize_tac(OptimizationLevel level);

/* Basic optimizations */
void constant_folding(void);
void constant_propagation(void);
void dead_code_elimination(void);
void copy_propagation(void);
void algebraic_simplification(void);

/* Peephole optimizations */
void peephole_optimization(void);
void remove_redundant_jumps(void);
void combine_operations(void);

/* Control flow optimizations */
void build_cfg(void);
void remove_unreachable_code(void);
void merge_basic_blocks(void);

/* Common subexpression elimination */
void common_subexpression_elimination(void);

/* Live variable analysis */
void live_variable_analysis(void);

/* Register allocation preparation */
void prepare_for_register_allocation(void);

/* Utility functions */
int is_constant(char *operand);
int get_constant_value(char *operand);
int is_temporary(char *operand);
void replace_operand(TACInstruction *instr, char *old_op, char *new_op);

/* Statistics */
typedef struct {
    int constants_folded;
    int dead_code_removed;
    int copies_propagated;
    int expressions_simplified;
    int subexpressions_eliminated;
    int original_instruction_count;
    int optimized_instruction_count;
} OptimizationStats;

extern OptimizationStats opt_stats;
void print_optimization_stats(void);

#endif /* OPTIMIZE_H */