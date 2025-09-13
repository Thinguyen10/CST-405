/*
 * Abstract Syntax Tree Implementation
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "globals.h"

extern int linenum;
extern int colnum;

/* Create a generic AST node */
ASTNode *create_node(NodeType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node == NULL) {
        fatal_error("Out of memory creating AST node");
    }
    
    node->node_type = type;
    node->data_type = TYPE_VOID;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->line_number = linenum;
    node->column_number = colnum;
    node->symbol = NULL;
    node->temp_var = -1;
    node->code = NULL;
    
    return node;
}

/* Create variable declaration node */
ASTNode *create_var_decl_node(char *type, char *name, int array_size) {
    ASTNode *node = create_node(NODE_VAR_DECL);
    
    if (strcmp(type, "int") == 0) {
        node->data_type = (array_size > 0) ? TYPE_ARRAY : TYPE_INT;
    } else {
        node->data_type = TYPE_VOID;
    }
    
    node->value.var_decl.name = copy_string(name);
    node->value.var_decl.size = array_size;
    
    return node;
}

/* Create function declaration node */
ASTNode *create_func_decl_node(char *type, char *name, ASTNode *params, ASTNode *body) {
    ASTNode *node = create_node(NODE_FUNC_DECL);
    
    node->data_type = TYPE_FUNCTION;
    node->value.func_decl.name = copy_string(name);
    
    if (strcmp(type, "int") == 0) {
        node->value.func_decl.return_type = TYPE_INT;
    } else {
        node->value.func_decl.return_type = TYPE_VOID;
    }
    
    node->left = params;
    node->right = body;
    
    return node;
}

/* Create parameter node */
ASTNode *create_param_node(char *type, char *name, int is_array) {
    ASTNode *node = create_node(NODE_PARAM);
    
    if (strcmp(type, "int") == 0) {
        node->data_type = is_array ? TYPE_ARRAY : TYPE_INT;
    } else {
        node->data_type = TYPE_VOID;
    }
    
    node->value.string_val = copy_string(name);
    
    return node;
}

/* Create compound statement node */
ASTNode *create_compound_stmt_node(ASTNode *local_decls, ASTNode *stmt_list) {
    ASTNode *node = create_node(NODE_COMPOUND_STMT);
    node->left = local_decls;
    node->right = stmt_list;
    return node;
}

/* Create if statement node */
ASTNode *create_if_node(ASTNode *condition, ASTNode *then_stmt, ASTNode *else_stmt) {
    ASTNode *node = create_node(NODE_IF_STMT);
    node->left = condition;
    node->right = then_stmt;
    node->next = else_stmt;
    return node;
}

/* Create while statement node */
ASTNode *create_while_node(ASTNode *condition, ASTNode *body) {
    ASTNode *node = create_node(NODE_WHILE_STMT);
    node->left = condition;
    node->right = body;
    return node;
}

/* Create return statement node */
ASTNode *create_return_node(ASTNode *expr) {
    ASTNode *node = create_node(NODE_RETURN_STMT);
    node->left = expr;
    return node;
}

/* Create assignment node */
ASTNode *create_assign_node(ASTNode *var, ASTNode *expr) {
    ASTNode *node = create_node(NODE_ASSIGN);
    node->left = var;
    node->right = expr;
    return node;
}

/* Create binary operation node */
ASTNode *create_binary_op_node(char *op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(NODE_BINARY_OP);
    node->value.string_val = copy_string(op);
    node->left = left;
    node->right = right;
    return node;
}

/* Create function call node */
ASTNode *create_call_node(char *name, ASTNode *args) {
    ASTNode *node = create_node(NODE_CALL);
    node->value.string_val = copy_string(name);
    node->left = args;
    return node;
}

/* Create identifier node */
ASTNode *create_id_node(char *name) {
    ASTNode *node = create_node(NODE_ID);
    node->value.string_val = copy_string(name);
    return node;
}

/* Create array access node */
ASTNode *create_array_access_node(char *name, ASTNode *index) {
    ASTNode *node = create_node(NODE_ARRAY_ACCESS);
    node->value.string_val = copy_string(name);
    node->left = index;
    return node;
}

/* Create number node */
ASTNode *create_num_node(int value) {
    ASTNode *node = create_node(NODE_NUM);
    node->value.int_val = value;
    node->data_type = TYPE_INT;
    return node;
}

/* Print AST in text format */
void print_ast(ASTNode *node, int indent) {
    if (node == NULL) return;
    
    /* Print indentation */
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    /* Print node type and value */
    switch (node->node_type) {
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            break;
        case NODE_VAR_DECL:
            printf("VAR_DECL: %s", node->value.var_decl.name);
            if (node->value.var_decl.size > 0) {
                printf("[%d]", node->value.var_decl.size);
            }
            printf("\n");
            break;
        case NODE_FUNC_DECL:
            printf("FUNC_DECL: %s\n", node->value.func_decl.name);
            break;
        case NODE_PARAM:
            printf("PARAM: %s\n", node->value.string_val);
            break;
        case NODE_COMPOUND_STMT:
            printf("COMPOUND_STMT\n");
            break;
        case NODE_IF_STMT:
            printf("IF_STMT\n");
            break;
        case NODE_WHILE_STMT:
            printf("WHILE_STMT\n");
            break;
        case NODE_RETURN_STMT:
            printf("RETURN_STMT\n");
            break;
        case NODE_ASSIGN:
            printf("ASSIGN\n");
            break;
        case NODE_BINARY_OP:
            printf("BINARY_OP: %s\n", node->value.string_val);
            break;
        case NODE_CALL:
            printf("CALL: %s\n", node->value.string_val);
            break;
        case NODE_ID:
            printf("ID: %s\n", node->value.string_val);
            break;
        case NODE_ARRAY_ACCESS:
            printf("ARRAY_ACCESS: %s\n", node->value.string_val);
            break;
        case NODE_NUM:
            printf("NUM: %d\n", node->value.int_val);
            break;
        default:
            printf("UNKNOWN_NODE\n");
    }
    
    /* Recursively print children */
    print_ast(node->left, indent + 1);
    print_ast(node->right, indent + 1);
    print_ast(node->next, indent);
}

/* Generate DOT format for GraphViz visualization */
void print_ast_dot_helper(ASTNode *node, FILE *file, int *node_id) {
    if (node == NULL) return;
    
    int current_id = (*node_id)++;
    
    /* Print node */
    fprintf(file, "  node%d [label=\"", current_id);
    
    switch (node->node_type) {
        case NODE_VAR_DECL:
            fprintf(file, "VAR: %s", node->value.var_decl.name);
            break;
        case NODE_FUNC_DECL:
            fprintf(file, "FUNC: %s", node->value.func_decl.name);
            break;
        case NODE_ID:
            fprintf(file, "ID: %s", node->value.string_val);
            break;
        case NODE_NUM:
            fprintf(file, "NUM: %d", node->value.int_val);
            break;
        case NODE_BINARY_OP:
            fprintf(file, "OP: %s", node->value.string_val);
            break;
        case NODE_IF_STMT:
            fprintf(file, "IF");
            break;
        case NODE_WHILE_STMT:
            fprintf(file, "WHILE");
            break;
        default:
            fprintf(file, "%d", node->node_type);
    }
    
    fprintf(file, "\"];\n");
    
    /* Print edges and recurse */
    if (node->left) {
        int left_id = *node_id;
        fprintf(file, "  node%d -> node%d [label=\"L\"];\n", current_id, left_id);
        print_ast_dot_helper(node->left, file, node_id);
    }
    
    if (node->right) {
        int right_id = *node_id;
        fprintf(file, "  node%d -> node%d [label=\"R\"];\n", current_id, right_id);
        print_ast_dot_helper(node->right, file, node_id);
    }
    
    if (node->next) {
        int next_id = *node_id;
        fprintf(file, "  node%d -> node%d [label=\"N\"];\n", current_id, next_id);
        print_ast_dot_helper(node->next, file, node_id);
    }
}

void print_ast_dot(ASTNode *node, FILE *file) {
    fprintf(file, "digraph AST {\n");
    fprintf(file, "  rankdir=TB;\n");
    fprintf(file, "  node [shape=box];\n");
    
    int node_id = 0;
    print_ast_dot_helper(node, file, &node_id);
    
    fprintf(file, "}\n");
}

/* Free AST memory */
void free_ast(ASTNode *node) {
    if (node == NULL) return;
    
    /* Free string values based on node type */
    switch (node->node_type) {
        case NODE_VAR_DECL:
            if (node->value.var_decl.name) {
                free(node->value.var_decl.name);
            }
            break;
            
        case NODE_FUNC_DECL:
            if (node->value.func_decl.name) {
                free(node->value.func_decl.name);
            }
            break;
            
        case NODE_ID:
        case NODE_ARRAY_ACCESS:
        case NODE_BINARY_OP:
        case NODE_CALL:
        case NODE_PARAM:
            if (node->value.string_val) {
                free(node->value.string_val);
            }
            break;
            
        default:
            /* Other node types don't have string values to free */
            break;
    }
    
    if (node->code) {
        free(node->code);
    }
    
    /* Recursively free children */
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    
    free(node);
}

/* Calculate AST height */
int get_ast_height(ASTNode *node) {
    if (node == NULL) return 0;
    
    int left_height = get_ast_height(node->left);
    int right_height = get_ast_height(node->right);
    int next_height = get_ast_height(node->next);
    
    int max_child = (left_height > right_height) ? left_height : right_height;
    max_child = (max_child > next_height) ? max_child : next_height;
    
    return 1 + max_child;
}

/* Count nodes in AST */
int count_ast_nodes(ASTNode *node) {
    if (node == NULL) return 0;
    
    return 1 + count_ast_nodes(node->left) + 
           count_ast_nodes(node->right) + 
           count_ast_nodes(node->next);
}

/* Demonstrate difference between parse tree and AST */
void show_parse_tree_vs_ast(void) {
    printf("\n=== Parse Tree vs Abstract Syntax Tree ===\n\n");
    
    printf("For the expression: x = 2 + 3 * 4\n\n");
    
    printf("PARSE TREE (includes all grammar productions):\n");
    printf("                    expression\n");
    printf("                         |\n");
    printf("                    assignment\n");
    printf("                    /    |    \\\n");
    printf("                  var   '='   simple-expr\n");
    printf("                   |               |\n");
    printf("                  ID:x        additive-expr\n");
    printf("                              /    |    \\\n");
    printf("                           term   '+'   term\n");
    printf("                            |            |\n");
    printf("                          factor    term '*' factor\n");
    printf("                            |         |        |\n");
    printf("                          NUM:2    factor    NUM:4\n");
    printf("                                      |\n");
    printf("                                    NUM:3\n\n");
    
    printf("ABSTRACT SYNTAX TREE (simplified, semantic structure):\n");
    printf("                    ASSIGN\n");
    printf("                    /    \\\n");
    printf("                 ID:x     +\n");
    printf("                        /   \\\n");
    printf("                     NUM:2   *\n");
    printf("                           /   \\\n");
    printf("                        NUM:3  NUM:4\n\n");
    
    printf("Key Differences:\n");
    printf("1. AST removes syntactic details (parentheses, keywords)\n");
    printf("2. AST focuses on semantic meaning\n");
    printf("3. AST is more compact and efficient for analysis\n");
    printf("4. AST directly represents operator precedence\n\n");
}