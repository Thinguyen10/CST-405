#ifndef SEMANTIC_H
#define SEMANTIC_H

/*
 * Semantic Analyzer for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include "ast.h"
#include "symtab.h"

/* Semantic analysis functions */
void semantic_analysis(ASTNode *tree);
void analyze_node(ASTNode *node);

/* Declaration analysis */
void analyze_var_decl(ASTNode *node);
void analyze_func_decl(ASTNode *node);
void analyze_params(ASTNode *params, SymbolEntry *func);

/* Statement analysis */
void analyze_compound_stmt(ASTNode *node);
void analyze_if_stmt(ASTNode *node);
void analyze_while_stmt(ASTNode *node);
void analyze_return_stmt(ASTNode *node);

/* Expression analysis and type checking */
DataType analyze_expression(ASTNode *node);
DataType analyze_binary_op(ASTNode *node);
DataType analyze_assignment(ASTNode *node);
DataType analyze_call(ASTNode *node);
DataType analyze_var(ASTNode *node);

/* Type checking utilities */
DataType check_type_compatibility(DataType t1, DataType t2, char *op);
int types_compatible(DataType t1, DataType t2);
char *type_to_string(DataType type);

/* Semantic checks */
void check_main_function(void);
void check_return_paths(ASTNode *func_body, DataType return_type);
void check_array_index(ASTNode *node);
void check_function_args(SymbolEntry *func, ASTNode *args);

/* Current function context */
extern SymbolEntry *current_function;

/* Error reporting */
void semantic_error(ASTNode *node, const char *format, ...);
void semantic_warning(ASTNode *node, const char *format, ...);

#endif /* SEMANTIC_H */