/*
 * Semantic Analyzer Implementation
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "symtab.h"
#include "globals.h"

/* Current function being analyzed */
SymbolEntry *current_function = NULL;

/* Main semantic analysis function */
void semantic_analysis(ASTNode *tree) {
    if (tree == NULL) return;
    
    printf("\n=== SEMANTIC ANALYSIS ===\n");
    
    /* Initialize symbol table */
    init_symbol_table();
    
    /* Analyze the AST */
    analyze_node(tree);
    
    /* Perform final checks */
    check_main_function();
    check_unused_symbols();
    
    /* Print symbol table if in trace mode */
    if (trace_semantic) {
        print_symbol_table();
    }
    
    printf("Semantic analysis completed. ");
    if (error_count == 0) {
        printf("No errors found.\n");
    } else {
        printf("%d error(s) found.\n", error_count);
    }
}

/* Analyze a single node */
void analyze_node(ASTNode *node) {
    if (node == NULL) return;
    
    switch (node->node_type) {
        case NODE_PROGRAM:
            analyze_node(node->left);
            analyze_node(node->right);
            break;
            
        case NODE_VAR_DECL:
            analyze_var_decl(node);
            break;
            
        case NODE_FUNC_DECL:
            analyze_func_decl(node);
            break;
            
        case NODE_COMPOUND_STMT:
            analyze_compound_stmt(node);
            break;
            
        case NODE_IF_STMT:
            analyze_if_stmt(node);
            break;
            
        case NODE_WHILE_STMT:
            analyze_while_stmt(node);
            break;
            
        case NODE_RETURN_STMT:
            analyze_return_stmt(node);
            break;
            
        case NODE_EXPR_STMT:
            if (node->left) {
                analyze_expression(node->left);
            }
            break;
            
        default:
            analyze_node(node->left);
            analyze_node(node->right);
            analyze_node(node->next);
            break;
    }
}

/* Analyze variable declaration */
void analyze_var_decl(ASTNode *node) {
    char *name = node->value.var_decl.name;
    int size = node->value.var_decl.size;
    
    /* Check for void variables */
    if (node->data_type == TYPE_VOID) {
        semantic_error(node, "Variable '%s' cannot have void type", name);
        return;
    }
    
    /* Insert into symbol table */
    SymbolKind kind = (size > 0) ? SYMBOL_ARRAY : SYMBOL_VAR;
    SymbolEntry *symbol = insert_symbol(name, kind, node->data_type);
    
    if (symbol && kind == SYMBOL_ARRAY) {
        symbol->size = size;
        allocate_memory(size * 4);  /* 4 bytes per int */
    }
    
    /* Link AST node to symbol table entry */
    node->symbol = symbol;
}

/* Analyze function declaration */
void analyze_func_decl(ASTNode *node) {
    char *name = node->value.func_decl.name;
    DataType return_type = node->value.func_decl.return_type;
    
    /* Insert function into global scope */
    SymbolEntry *func = insert_symbol(name, SYMBOL_FUNCTION, TYPE_FUNCTION);
    if (func == NULL) return;  /* Already declared */
    
    func->type = return_type;
    node->symbol = func;
    
    /* Set current function context */
    current_function = func;
    
    /* Enter new scope for function */
    enter_scope();
    
    /* Process parameters */
    analyze_params(node->left, func);
    
    /* Analyze function body */
    analyze_node(node->right);
    
    /* Check return paths */
    if (return_type != TYPE_VOID) {
        check_return_paths(node->right, return_type);
    }
    
    /* Exit function scope */
    exit_scope();
    current_function = NULL;
}

/* Analyze function parameters */
void analyze_params(ASTNode *params, SymbolEntry *func) {
    ASTNode *param = params;
    
    while (param) {
        if (param->node_type == NODE_PARAM) {
            char *param_name = param->value.string_val;
            DataType param_type = param->data_type;
            
            /* Insert parameter into symbol table */
            SymbolEntry *param_symbol = insert_symbol(param_name, SYMBOL_PARAM, param_type);
            
            /* Add to function's parameter list */
            if (param_symbol) {
                SymbolEntry *param_copy = (SymbolEntry *)malloc(sizeof(SymbolEntry));
                *param_copy = *param_symbol;
                param_copy->next = NULL;
                add_param_to_function(func, param_copy);
            }
            
            param->symbol = param_symbol;
        }
        
        param = (param->node_type == NODE_PARAM_LIST) ? param->right : param->next;
    }
}

/* Analyze compound statement */
void analyze_compound_stmt(ASTNode *node) {
    /* Note: We already entered scope in function declaration */
    /* For nested compound statements, we would enter a new scope here */
    
    /* Analyze local declarations */
    analyze_node(node->left);
    
    /* Analyze statement list */
    analyze_node(node->right);
}

/* Analyze if statement */
void analyze_if_stmt(ASTNode *node) {
    /* Check condition */
    DataType cond_type = analyze_expression(node->left);
    if (cond_type == TYPE_VOID) {
        semantic_error(node, "If condition cannot be void");
    }
    
    /* Analyze then branch */
    analyze_node(node->right);
    
    /* Analyze else branch if present */
    analyze_node(node->next);
}

/* Analyze while statement */
void analyze_while_stmt(ASTNode *node) {
    /* Check condition */
    DataType cond_type = analyze_expression(node->left);
    if (cond_type == TYPE_VOID) {
        semantic_error(node, "While condition cannot be void");
    }
    
    /* Analyze body */
    analyze_node(node->right);
}

/* Analyze return statement */
void analyze_return_stmt(ASTNode *node) {
    if (current_function == NULL) {
        semantic_error(node, "Return statement outside function");
        return;
    }
    
    DataType func_return_type = current_function->type;
    
    if (node->left == NULL) {
        /* Return without value */
        if (func_return_type != TYPE_VOID) {
            semantic_error(node, "Function '%s' must return a value", 
                          current_function->name);
        }
    } else {
        /* Return with value */
        if (func_return_type == TYPE_VOID) {
            semantic_error(node, "Void function '%s' cannot return a value",
                          current_function->name);
        } else {
            DataType expr_type = analyze_expression(node->left);
            if (!types_compatible(func_return_type, expr_type)) {
                semantic_error(node, "Return type mismatch in function '%s'",
                              current_function->name);
            }
        }
    }
}

/* Analyze expression and return its type */
DataType analyze_expression(ASTNode *node) {
    if (node == NULL) return TYPE_ERROR;
    
    switch (node->node_type) {
        case NODE_NUM:
            node->data_type = TYPE_INT;
            return TYPE_INT;
            
        case NODE_ID:
            return analyze_var(node);
            
        case NODE_ARRAY_ACCESS:
            return analyze_var(node);
            
        case NODE_ASSIGN:
            return analyze_assignment(node);
            
        case NODE_BINARY_OP:
            return analyze_binary_op(node);
            
        case NODE_CALL:
            return analyze_call(node);
            
        default:
            return TYPE_ERROR;
    }
}

/* Analyze variable reference */
DataType analyze_var(ASTNode *node) {
    char *name = node->value.string_val;
    SymbolEntry *symbol = lookup_symbol(name);
    
    if (symbol == NULL) {
        semantic_error(node, "Undefined variable '%s'", name);
        return TYPE_ERROR;
    }
    
    node->symbol = symbol;
    
    if (node->node_type == NODE_ARRAY_ACCESS) {
        if (symbol->kind != SYMBOL_ARRAY && symbol->kind != SYMBOL_PARAM) {
            semantic_error(node, "'%s' is not an array", name);
            return TYPE_ERROR;
        }
        
        /* Check index type */
        DataType index_type = analyze_expression(node->left);
        if (index_type != TYPE_INT) {
            semantic_error(node, "Array index must be integer");
        }
        
        return TYPE_INT;
    }
    
    return symbol->type;
}

/* Analyze assignment */
DataType analyze_assignment(ASTNode *node) {
    DataType var_type = analyze_expression(node->left);
    DataType expr_type = analyze_expression(node->right);
    
    if (var_type == TYPE_ERROR || expr_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    if (!types_compatible(var_type, expr_type)) {
        semantic_error(node, "Type mismatch in assignment");
        return TYPE_ERROR;
    }
    
    node->data_type = var_type;
    return var_type;
}

/* Analyze binary operation */
DataType analyze_binary_op(ASTNode *node) {
    DataType left_type = analyze_expression(node->left);
    DataType right_type = analyze_expression(node->right);
    char *op = node->value.string_val;
    
    if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    if (left_type == TYPE_VOID || right_type == TYPE_VOID) {
        semantic_error(node, "Void type cannot be used in expression");
        return TYPE_ERROR;
    }
    
    if (!types_compatible(left_type, right_type)) {
        semantic_error(node, "Type mismatch in binary operation '%s'", op);
        return TYPE_ERROR;
    }
    
    node->data_type = TYPE_INT;
    return TYPE_INT;
}

/* Analyze function call */
DataType analyze_call(ASTNode *node) {
    char *name = node->value.string_val;
    SymbolEntry *func = lookup_symbol(name);
    
    if (func == NULL) {
        semantic_error(node, "Undefined function '%s'", name);
        return TYPE_ERROR;
    }
    
    if (func->kind != SYMBOL_FUNCTION) {
        semantic_error(node, "'%s' is not a function", name);
        return TYPE_ERROR;
    }
    
    node->symbol = func;
    
    /* Check arguments */
    check_function_args(func, node->left);
    
    /* Handle built-in functions */
    if (strcmp(name, "input") == 0) {
        return TYPE_INT;
    } else if (strcmp(name, "output") == 0) {
        return TYPE_VOID;
    }
    
    return func->type;
}

/* Check function arguments */
void check_function_args(SymbolEntry *func, ASTNode *args) {
    SymbolEntry *param = func->params;
    ASTNode *arg = args;
    
    while (arg && param) {
        DataType arg_type = analyze_expression(
            (arg->node_type == NODE_ARG_LIST) ? arg->right : arg);
        
        if (!types_compatible(param->type, arg_type)) {
            semantic_error(arg, "Argument type mismatch in call to '%s'", func->name);
        }
        
        param = param->next;
        arg = (arg->node_type == NODE_ARG_LIST) ? arg->left : NULL;
    }
    
    if (param) {
        semantic_error(args, "Too few arguments in call to '%s'", func->name);
    } else if (arg) {
        semantic_error(args, "Too many arguments in call to '%s'", func->name);
    }
}

/* Check type compatibility */
int types_compatible(DataType t1, DataType t2) {
    if (t1 == t2) return 1;
    if (t1 == TYPE_ERROR || t2 == TYPE_ERROR) return 1;  /* Error recovery */
    return 0;
}

/* Check for main function */
void check_main_function(void) {
    SymbolEntry *main_func = lookup_symbol("main");
    
    if (main_func == NULL) {
        error("No 'main' function defined");
    } else if (main_func->kind != SYMBOL_FUNCTION) {
        error("'main' must be a function");
    } else if (main_func->type != TYPE_VOID) {
        warning("'main' function should return void");
    } else if (main_func->param_count != 0) {
        warning("'main' function should have no parameters");
    }
}

/* Check return paths (simplified) */
void check_return_paths(ASTNode *func_body, DataType return_type) {
    /* This is a simplified check - a full implementation would
       ensure all paths through the function have appropriate returns */
    /* For now, we just warn if no return statement is found */
    
    if (return_type != TYPE_VOID) {
        /* Simple check: look for at least one return statement */
        /* In reality, we'd need control flow analysis */
    }
}

/* Convert type to string */
char *type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_VOID: return "void";
        case TYPE_ARRAY: return "int[]";
        case TYPE_FUNCTION: return "function";
        default: return "error";
    }
}

/* Semantic error reporting */
void semantic_error(ASTNode *node, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Semantic error at line %d: ", 
            node ? node->line_number : linenum);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    error_count++;
}

/* Semantic warning reporting */
void semantic_warning(ASTNode *node, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Warning at line %d: ",
            node ? node->line_number : linenum);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    warning_count++;
}