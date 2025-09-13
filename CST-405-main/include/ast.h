#ifndef AST_H
#define AST_H

/*
 * Abstract Syntax Tree (AST) definitions for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include "globals.h"

/* AST Node Types */
typedef enum {
    /* Program structure */
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_FUNC_DECL,
    NODE_PARAM,
    NODE_PARAM_LIST,
    
    /* Statements */
    NODE_COMPOUND_STMT,
    NODE_LOCAL_DECLS,
    NODE_STMT_LIST,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_RETURN_STMT,
    NODE_EXPR_STMT,
    
    /* Expressions */
    NODE_ASSIGN,
    NODE_BINARY_OP,
    NODE_CALL,
    NODE_ARG_LIST,
    
    /* Variables and constants */
    NODE_ID,
    NODE_ARRAY_ACCESS,
    NODE_NUM,
    
    /* Type nodes */
    NODE_TYPE_INT,
    NODE_TYPE_VOID
} NodeType;

/* Type information */
typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_ERROR
} DataType;

/* AST Node Structure */
typedef struct ASTNode {
    NodeType node_type;
    DataType data_type;
    
    /* Node specific data */
    union {
        int int_val;           /* For NUM nodes */
        char *string_val;      /* For ID nodes and operators */
        struct {
            char *name;
            int size;          /* For array declarations */
        } var_decl;
        struct {
            char *name;
            DataType return_type;
        } func_decl;
    } value;
    
    /* Tree structure */
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;      /* For lists */
    
    /* Source location */
    int line_number;
    int column_number;
    
    /* Symbol table entry (filled during semantic analysis) */
    void *symbol;
    
    /* Code generation attributes */
    int temp_var;              /* Temporary variable number */
    char *code;                /* Generated code */
    
} ASTNode;

/* AST Construction Functions */
ASTNode *create_node(NodeType type);
ASTNode *create_var_decl_node(char *type, char *name, int array_size);
ASTNode *create_func_decl_node(char *type, char *name, ASTNode *params, ASTNode *body);
ASTNode *create_param_node(char *type, char *name, int is_array);
ASTNode *create_compound_stmt_node(ASTNode *local_decls, ASTNode *stmt_list);
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_stmt, ASTNode *else_stmt);
ASTNode *create_while_node(ASTNode *condition, ASTNode *body);
ASTNode *create_return_node(ASTNode *expr);
ASTNode *create_assign_node(ASTNode *var, ASTNode *expr);
ASTNode *create_binary_op_node(char *op, ASTNode *left, ASTNode *right);
ASTNode *create_call_node(char *name, ASTNode *args);
ASTNode *create_id_node(char *name);
ASTNode *create_array_access_node(char *name, ASTNode *index);
ASTNode *create_num_node(int value);

/* AST Traversal and Display */
void print_ast(ASTNode *node, int indent);
void print_ast_dot(ASTNode *node, FILE *file);
void free_ast(ASTNode *node);

/* AST Analysis */
int get_ast_height(ASTNode *node);
int count_ast_nodes(ASTNode *node);

/* Parse Tree vs AST Demonstration */
void show_parse_tree_vs_ast(void);

#endif /* AST_H */