/*
 * MIPS Code Generator Implementation
 * CST-405 Compiler Design
 * 
 * Generates MIPS assembly from three-address code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "mips.h"
#include "codegen.h"
#include "symtab.h"
#include "globals.h"
#include "optimize.h"

/* Global MIPS context */
static MIPSContext *mips_ctx = NULL;

/* Register names */
static const char *register_names[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

/* Main MIPS generation function */
void generate_mips(TACInstruction *tac_list, FILE *output) {
    printf("\n=== MIPS CODE GENERATION ===\n");
    
    /* Initialize context */
    mips_ctx = (MIPSContext *)malloc(sizeof(MIPSContext));
    mips_ctx->output = output;
    mips_ctx->stack_offset = 0;
    mips_ctx->param_offset = 0;
    mips_ctx->current_func = NULL;
    
    /* Initialize register table */
    for (int i = 0; i < 32; i++) {
        mips_ctx->regs[i].var_name = NULL;
        mips_ctx->regs[i].is_dirty = 0;
        mips_ctx->regs[i].last_use = 0;
    }
    
    /* Generate data section */
    emit_data_section();
    
    /* Generate text section */
    emit_text_section();
    
    /* Generate code for each TAC instruction */
    TACInstruction *instr = tac_list;
    while (instr) {
        gen_mips_instruction(instr);
        instr = instr->next;
    }
    
    /* Generate syscall functions */
    emit_syscall_functions();
    
    printf("MIPS code generation completed.\n");
}

/* Generate MIPS for a single TAC instruction */
void gen_mips_instruction(TACInstruction *instr) {
    switch (instr->opcode) {
        case TAC_ADD:
        case TAC_SUB:
        case TAC_MUL:
        case TAC_DIV:
            gen_mips_arithmetic(instr);
            break;
            
        case TAC_ASSIGN:
        case TAC_LOAD_CONST:
            gen_mips_assignment(instr);
            break;
            
        case TAC_LT:
        case TAC_LTE:
        case TAC_GT:
        case TAC_GTE:
        case TAC_EQ:
        case TAC_NEQ:
            gen_mips_comparison(instr);
            break;
            
        case TAC_GOTO:
        case TAC_IF_TRUE:
        case TAC_IF_FALSE:
            gen_mips_branch(instr);
            break;
            
        case TAC_LABEL:
            emit_label_mips(instr->label);
            break;
            
        case TAC_FUNC_BEGIN:
        case TAC_FUNC_END:
            gen_mips_function(instr);
            break;
            
        case TAC_CALL:
        case TAC_PARAM:
            gen_mips_call(instr);
            break;
            
        case TAC_RETURN:
            gen_mips_return(instr);
            break;
            
        case TAC_ARRAY_LOAD:
        case TAC_ARRAY_STORE:
            gen_mips_array(instr);
            break;
            
        default:
            emit_mips("    # Unknown TAC opcode\n");
    }
}

/* Generate MIPS arithmetic operations */
void gen_mips_arithmetic(TACInstruction *instr) {
    MIPSRegister rd = allocate_register(instr->result);
    MIPSRegister rs = get_register(instr->arg1);
    MIPSRegister rt = get_register(instr->arg2);
    
    switch (instr->opcode) {
        case TAC_ADD:
            emit_mips("    add %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_SUB:
            emit_mips("    sub %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_MUL:
            emit_mips("    mul %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_DIV:
            emit_mips("    div %s, %s\n", reg_name(rs), reg_name(rt));
            emit_mips("    mflo %s\n", reg_name(rd));
            break;
        default:
            break;
    }
    
    mips_ctx->regs[rd].is_dirty = 1;
}

/* Generate MIPS assignment */
void gen_mips_assignment(TACInstruction *instr) {
    if (instr->opcode == TAC_LOAD_CONST) {
        /* Load constant */
        MIPSRegister rd = allocate_register(instr->result);
        int value = atoi(instr->arg1);
        emit_mips("    li %s, %d\n", reg_name(rd), value);
        mips_ctx->regs[rd].is_dirty = 1;
    } else {
        /* Copy assignment */
        MIPSRegister rs = get_register(instr->arg1);
        MIPSRegister rd = allocate_register(instr->result);
        
        if (rd != rs) {
            emit_mips("    move %s, %s\n", reg_name(rd), reg_name(rs));
            mips_ctx->regs[rd].is_dirty = 1;
        }
    }
}

/* Generate MIPS comparison */
void gen_mips_comparison(TACInstruction *instr) {
    MIPSRegister rd = allocate_register(instr->result);
    MIPSRegister rs = get_register(instr->arg1);
    MIPSRegister rt = get_register(instr->arg2);
    
    switch (instr->opcode) {
        case TAC_LT:
            emit_mips("    slt %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_LTE:
            emit_mips("    sle %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_GT:
            emit_mips("    sgt %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_GTE:
            emit_mips("    sge %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_EQ:
            emit_mips("    seq %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        case TAC_NEQ:
            emit_mips("    sne %s, %s, %s\n", reg_name(rd), reg_name(rs), reg_name(rt));
            break;
        default:
            break;
    }
    
    mips_ctx->regs[rd].is_dirty = 1;
}

/* Generate MIPS branch */
void gen_mips_branch(TACInstruction *instr) {
    if (instr->opcode == TAC_GOTO) {
        emit_mips("    j L%d\n", instr->label);
    } else {
        MIPSRegister rs = get_register(instr->result);
        
        if (instr->opcode == TAC_IF_TRUE) {
            emit_mips("    bnez %s, L%d\n", reg_name(rs), instr->label);
        } else {
            emit_mips("    beqz %s, L%d\n", reg_name(rs), instr->label);
        }
    }
}

/* Generate MIPS function prologue/epilogue */
void gen_mips_function(TACInstruction *instr) {
    if (instr->opcode == TAC_FUNC_BEGIN) {
        mips_ctx->current_func = instr->result;
        mips_ctx->stack_offset = 0;
        
        emit_mips("\n%s:\n", instr->result);
        
        /* Function prologue */
        emit_mips("    # Function prologue\n");
        emit_mips("    addi $sp, $sp, -32\n");  /* Allocate stack frame */
        emit_mips("    sw $ra, 28($sp)\n");      /* Save return address */
        emit_mips("    sw $fp, 24($sp)\n");      /* Save frame pointer */
        emit_mips("    move $fp, $sp\n");        /* Set new frame pointer */
        
    } else if (instr->opcode == TAC_FUNC_END) {
        /* Function epilogue */
        emit_mips("    # Function epilogue\n");
        emit_mips("    move $sp, $fp\n");        /* Restore stack pointer */
        emit_mips("    lw $fp, 24($sp)\n");      /* Restore frame pointer */
        emit_mips("    lw $ra, 28($sp)\n");      /* Restore return address */
        emit_mips("    addi $sp, $sp, 32\n");    /* Deallocate stack frame */
        
        if (strcmp(mips_ctx->current_func, "main") == 0) {
            /* Exit for main function */
            emit_mips("    li $v0, 10\n");       /* Exit syscall */
            emit_mips("    syscall\n");
        } else {
            emit_mips("    jr $ra\n");           /* Return */
        }
    }
}

/* Generate MIPS function call */
void gen_mips_call(TACInstruction *instr) {
    if (instr->opcode == TAC_PARAM) {
        /* Push parameter */
        MIPSRegister rs = get_register(instr->result);
        
        if (mips_ctx->param_offset < 4) {
            /* First 4 parameters in $a0-$a3 */
            emit_mips("    move $a%d, %s\n", mips_ctx->param_offset, reg_name(rs));
        } else {
            /* Additional parameters on stack */
            int offset = (mips_ctx->param_offset - 4) * 4;
            emit_mips("    sw %s, %d($sp)\n", reg_name(rs), offset);
        }
        mips_ctx->param_offset++;
        
    } else if (instr->opcode == TAC_CALL) {
        /* Make the call */
        if (strcmp(instr->arg1, "input") == 0) {
            /* Built-in input function */
            emit_mips("    jal _input\n");
            if (instr->result) {
                MIPSRegister rd = allocate_register(instr->result);
                emit_mips("    move %s, $v0\n", reg_name(rd));
                mips_ctx->regs[rd].is_dirty = 1;
            }
        } else if (strcmp(instr->arg1, "output") == 0) {
            /* Built-in output function */
            emit_mips("    jal _output\n");
        } else {
            /* User-defined function */
            emit_mips("    jal %s\n", instr->arg1);
            if (instr->result) {
                MIPSRegister rd = allocate_register(instr->result);
                emit_mips("    move %s, $v0\n", reg_name(rd));
                mips_ctx->regs[rd].is_dirty = 1;
            }
        }
        mips_ctx->param_offset = 0;  /* Reset parameter count */
    }
}

/* Generate MIPS return */
void gen_mips_return(TACInstruction *instr) {
    if (instr->result) {
        MIPSRegister rs = get_register(instr->result);
        emit_mips("    move $v0, %s\n", reg_name(rs));
    }
    emit_mips("    j %s_exit\n", mips_ctx->current_func);
    emit_mips("%s_exit:\n", mips_ctx->current_func);
}

/* Generate MIPS array operations */
void gen_mips_array(TACInstruction *instr) {
    if (instr->opcode == TAC_ARRAY_LOAD) {
        /* t = a[i] */
        MIPSRegister index = get_register(instr->arg2);
        MIPSRegister temp = allocate_register("$t8");
        MIPSRegister result = allocate_register(instr->result);
        
        /* Calculate address: base + 4 * index */
        emit_mips("    sll %s, %s, 2\n", reg_name(temp), reg_name(index));
        
        if (is_global_var(instr->arg1)) {
            emit_mips("    la $t9, %s\n", instr->arg1);
            emit_mips("    add %s, $t9, %s\n", reg_name(temp), reg_name(temp));
        } else {
            int offset = get_var_offset(instr->arg1);
            emit_mips("    addi %s, $fp, %d\n", reg_name(temp), offset);
            emit_mips("    add %s, %s, %s\n", reg_name(temp), reg_name(temp), reg_name(index));
        }
        
        emit_mips("    lw %s, 0(%s)\n", reg_name(result), reg_name(temp));
        mips_ctx->regs[result].is_dirty = 1;
        
    } else if (instr->opcode == TAC_ARRAY_STORE) {
        /* a[i] = t */
        MIPSRegister index = get_register(instr->arg1);
        MIPSRegister value = get_register(instr->arg2);
        MIPSRegister temp = allocate_register("$t8");
        
        /* Calculate address: base + 4 * index */
        emit_mips("    sll %s, %s, 2\n", reg_name(temp), reg_name(index));
        
        if (is_global_var(instr->result)) {
            emit_mips("    la $t9, %s\n", instr->result);
            emit_mips("    add %s, $t9, %s\n", reg_name(temp), reg_name(temp));
        } else {
            int offset = get_var_offset(instr->result);
            emit_mips("    addi %s, $fp, %d\n", reg_name(temp), offset);
            emit_mips("    add %s, %s, %s\n", reg_name(temp), reg_name(temp), reg_name(index));
        }
        
        emit_mips("    sw %s, 0(%s)\n", reg_name(value), reg_name(temp));
    }
}

/* Simple register allocation */
MIPSRegister allocate_register(char *var) {
    /* Simple allocation: use $t0-$t7 for temporaries */
    static int next_temp = 0;
    
    if (var && var[0] == 't' && var[1] >= '0' && var[1] <= '9') {
        /* Temporary variable */
        int temp_num = atoi(var + 1);
        MIPSRegister reg = REG_T0 + (temp_num % 8);
        
        if (mips_ctx->regs[reg].var_name && 
            strcmp(mips_ctx->regs[reg].var_name, var) != 0) {
            /* Spill if necessary */
            spill_register(reg);
        }
        
        mips_ctx->regs[reg].var_name = copy_string(var);
        return reg;
    } else {
        /* Regular variable - use saved registers */
        MIPSRegister reg = REG_S0 + (next_temp++ % 8);
        
        if (mips_ctx->regs[reg].var_name) {
            spill_register(reg);
        }
        
        if (var) {
            mips_ctx->regs[reg].var_name = copy_string(var);
            load_variable(var, reg);
        }
        
        return reg;
    }
}

/* Get register containing variable */
MIPSRegister get_register(char *var) {
    if (is_constant(var)) {
        /* Load constant into register */
        MIPSRegister reg = REG_T8;
        int value = atoi(var);
        emit_mips("    li %s, %d\n", reg_name(reg), value);
        return reg;
    }
    
    /* Check if already in a register */
    for (int i = REG_T0; i <= REG_S7; i++) {
        if (mips_ctx->regs[i].var_name && 
            strcmp(mips_ctx->regs[i].var_name, var) == 0) {
            return i;
        }
    }
    
    /* Not in register, allocate one */
    return allocate_register(var);
}

/* Spill register to memory */
void spill_register(MIPSRegister reg) {
    if (mips_ctx->regs[reg].is_dirty) {
        store_variable(mips_ctx->regs[reg].var_name, reg);
    }
    free(mips_ctx->regs[reg].var_name);
    mips_ctx->regs[reg].var_name = NULL;
    mips_ctx->regs[reg].is_dirty = 0;
}

/* Load variable from memory */
void load_variable(char *var, MIPSRegister reg) {
    if (is_global_var(var)) {
        emit_mips("    lw %s, %s\n", reg_name(reg), var);
    } else {
        int offset = get_var_offset(var);
        emit_mips("    lw %s, %d($fp)\n", reg_name(reg), offset);
    }
}

/* Store variable to memory */
void store_variable(char *var, MIPSRegister reg) {
    if (is_global_var(var)) {
        emit_mips("    sw %s, %s\n", reg_name(reg), var);
    } else {
        int offset = get_var_offset(var);
        emit_mips("    sw %s, %d($fp)\n", reg_name(reg), offset);
    }
}

/* Emit MIPS instruction */
void emit_mips(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(mips_ctx->output, format, args);
    va_end(args);
}

/* Emit label */
void emit_label_mips(int label) {
    emit_mips("L%d:\n", label);
}

/* Emit data section */
void emit_data_section(void) {
    emit_mips("# C-Minus Compiler Generated MIPS Code\n");
    emit_mips("# CST-405 Compiler Design\n\n");
    emit_mips(".data\n");
    emit_mips("newline: .asciiz \"\\n\"\n");
    emit_mips("prompt: .asciiz \"Enter a number: \"\n");
    
    /* Add global variables here */
    /* This would be populated from the symbol table */
    
    emit_mips("\n");
}

/* Emit text section */
void emit_text_section(void) {
    emit_mips(".text\n");
    emit_mips(".globl main\n\n");
}

/* Emit syscall functions */
void emit_syscall_functions(void) {
    /* Input function */
    emit_mips("\n_input:\n");
    emit_mips("    li $v0, 4\n");          /* Print string syscall */
    emit_mips("    la $a0, prompt\n");
    emit_mips("    syscall\n");
    emit_mips("    li $v0, 5\n");          /* Read integer syscall */
    emit_mips("    syscall\n");
    emit_mips("    jr $ra\n");
    
    /* Output function */
    emit_mips("\n_output:\n");
    emit_mips("    li $v0, 1\n");          /* Print integer syscall */
    emit_mips("    syscall\n");
    emit_mips("    li $v0, 4\n");          /* Print string syscall */
    emit_mips("    la $a0, newline\n");
    emit_mips("    syscall\n");
    emit_mips("    jr $ra\n");
}

/* Get register name */
char *reg_name(MIPSRegister reg) {
    return (char *)register_names[reg];
}

/* Get variable offset (simplified) */
int get_var_offset(char *var) {
    /* This would be looked up from the symbol table */
    /* For now, return a default offset */
    return -4;
}

/* Check if variable is global */
int is_global_var(char *var) {
    /* This would be checked from the symbol table */
    /* For now, assume locals start with 't' or are parameters */
    return !(var[0] == 't' || var[0] == 'p');
}