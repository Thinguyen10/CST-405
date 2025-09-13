#ifndef MIPS_H
#define MIPS_H

/*
 * MIPS Code Generator for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include "codegen.h"
#include "symtab.h"

/* MIPS Registers */
typedef enum {
    /* Zero register */
    REG_ZERO = 0,   /* $0 - always zero */
    
    /* Assembler temporary */
    REG_AT = 1,     /* $1 - assembler temporary */
    
    /* Function results */
    REG_V0 = 2,     /* $2 - function result */
    REG_V1 = 3,     /* $3 - function result */
    
    /* Function arguments */
    REG_A0 = 4,     /* $4 - first argument */
    REG_A1 = 5,     /* $5 - second argument */
    REG_A2 = 6,     /* $6 - third argument */
    REG_A3 = 7,     /* $7 - fourth argument */
    
    /* Temporaries */
    REG_T0 = 8,     /* $8 - temporary */
    REG_T1 = 9,     /* $9 - temporary */
    REG_T2 = 10,    /* $10 - temporary */
    REG_T3 = 11,    /* $11 - temporary */
    REG_T4 = 12,    /* $12 - temporary */
    REG_T5 = 13,    /* $13 - temporary */
    REG_T6 = 14,    /* $14 - temporary */
    REG_T7 = 15,    /* $15 - temporary */
    
    /* Saved temporaries */
    REG_S0 = 16,    /* $16 - saved temporary */
    REG_S1 = 17,    /* $17 - saved temporary */
    REG_S2 = 18,    /* $18 - saved temporary */
    REG_S3 = 19,    /* $19 - saved temporary */
    REG_S4 = 20,    /* $20 - saved temporary */
    REG_S5 = 21,    /* $21 - saved temporary */
    REG_S6 = 22,    /* $22 - saved temporary */
    REG_S7 = 23,    /* $23 - saved temporary */
    
    /* More temporaries */
    REG_T8 = 24,    /* $24 - temporary */
    REG_T9 = 25,    /* $25 - temporary */
    
    /* Kernel registers */
    REG_K0 = 26,    /* $26 - kernel */
    REG_K1 = 27,    /* $27 - kernel */
    
    /* Global pointer */
    REG_GP = 28,    /* $28 - global pointer */
    
    /* Stack pointer */
    REG_SP = 29,    /* $29 - stack pointer */
    
    /* Frame pointer */
    REG_FP = 30,    /* $30 - frame pointer */
    
    /* Return address */
    REG_RA = 31     /* $31 - return address */
} MIPSRegister;

/* Register allocation info */
typedef struct {
    char *var_name;         /* Variable/temp mapped to this register */
    int is_dirty;           /* Need to write back to memory */
    int last_use;           /* Last instruction that used this register */
} RegisterInfo;

/* MIPS generation context */
typedef struct {
    FILE *output;           /* Output file */
    RegisterInfo regs[32]; /* Register allocation table */
    int stack_offset;       /* Current stack offset */
    int param_offset;       /* Parameter offset */
    char *current_func;     /* Current function name */
} MIPSContext;

/* Main MIPS generation function */
void generate_mips(TACInstruction *tac_list, FILE *output);

/* MIPS instruction generation */
void gen_mips_instruction(TACInstruction *instr);
void gen_mips_arithmetic(TACInstruction *instr);
void gen_mips_assignment(TACInstruction *instr);
void gen_mips_comparison(TACInstruction *instr);
void gen_mips_branch(TACInstruction *instr);
void gen_mips_function(TACInstruction *instr);
void gen_mips_call(TACInstruction *instr);
void gen_mips_return(TACInstruction *instr);
void gen_mips_array(TACInstruction *instr);

/* Register allocation */
MIPSRegister allocate_register(char *var);
MIPSRegister get_register(char *var);
void free_register(MIPSRegister reg);
void spill_register(MIPSRegister reg);
void load_variable(char *var, MIPSRegister reg);
void store_variable(char *var, MIPSRegister reg);

/* MIPS output functions */
void emit_mips(const char *format, ...);
void emit_label_mips(int label);
void emit_data_section(void);
void emit_text_section(void);
void emit_syscall_functions(void);

/* Utility functions */
char *reg_name(MIPSRegister reg);
int get_var_offset(char *var);
int is_global_var(char *var);

#endif /* MIPS_H */