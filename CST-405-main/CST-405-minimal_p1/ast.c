/* AST IMPLEMENTATION
 * Functions to create and manipulate Abstract Syntax Tree nodes
 * The AST is built during parsing and used for all subsequent phases
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Create a number literal node */
ASTNode* createNum(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_NUM;
    node->data.num = value;
    return node;
}

/* Create a variable reference node */
ASTNode* createVar(char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VAR;
    node->data.name = strdup(name);
    return node;
}

/* Create a binary operation node */
ASTNode* createBinOp(const char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINOP;
    node->data.binop.op = strdup(op);
    node->data.binop.left = left;
    node->data.binop.right = right;
    return node;
}

/* Create a unary operation node */
ASTNode* createUnaryOp(const char* op, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_UNARY;
    node->data.unary.op = strdup(op);
    node->data.unary.expr = expr;
    return node;
}

/* Create a variable declaration node */
ASTNode* createDecl(char* name, ASTNode* init) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_DECL;
    node->data.decl.name = strdup(name);
    node->data.decl.init = init;
    return node;
}

/* Create an assignment statement node */
ASTNode* createAssign(char* var, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ASSIGN;
    node->data.assign.var = strdup(var);
    node->data.assign.value = value;
    return node;
}

/* Create a print statement node */
ASTNode* createPrint(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->data.expr = expr;
    return node;
}

/* Create a statement list node */
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_STMT_LIST;
    node->data.stmtlist.stmt = stmt1;
    node->data.stmtlist.next = stmt2;
    return node;
}

/* Expression list */
ASTNode* createExprList(ASTNode* first, ASTNode* rest) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_EXPR_LIST;
    node->data.exprlist.first = first;
    node->data.exprlist.next = rest;
    return node;
}

ASTNode* addToExprList(ASTNode* list, ASTNode* expr) {
    if (!list) return createExprList(expr, NULL);
    ASTNode* cur = list;
    while (cur->data.exprlist.next)
        cur = cur->data.exprlist.next;
    cur->data.exprlist.next = createExprList(expr, NULL);
    return list;
}

/* Break statement */
ASTNode* createBreak() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BREAK;
    return node;
}

/* If statement */
ASTNode* createIf(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->data.if_stmt.cond = cond;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

/* While statement */
ASTNode* createWhile(ASTNode* cond, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_WHILE;
    node->data.while_stmt.cond = cond;
    node->data.while_stmt.body = body;
    return node;
}

/* For statement */
ASTNode* createFor(ASTNode* init, ASTNode* cond, ASTNode* update, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FOR;
    node->data.for_stmt.init = init;
    node->data.for_stmt.cond = cond;
    node->data.for_stmt.update = update;
    node->data.for_stmt.body = body;
    return node;
}

/* Return statement */
ASTNode* createReturn(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_RETURN;
    node->data.ret_expr = expr;
    return node;
}

/* Display AST (debugging) */
void printAST(ASTNode* node, int level) {
    if (!node) return;

    for (int i = 0; i < level; i++) printf("  ");

    switch(node->type) {
        case NODE_NUM:
            printf("NUM: %lf\n", node->data.num);
            break;
        case NODE_VAR:
            printf("VAR: %s\n", node->data.name);
            break;
        case NODE_BINOP:
            printf("BINOP: %s\n", node->data.binop.op);
            printAST(node->data.binop.left, level+1);
            printAST(node->data.binop.right, level+1);
            break;
        case NODE_UNARY:
            printf("UNARY: %s\n", node->data.unary.op);
            printAST(node->data.unary.expr, level+1);
            break;
        case NODE_DECL:
            printf("DECL: %s\n", node->data.decl.name);
            if (node->data.decl.init) printAST(node->data.decl.init, level+1);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN: %s\n", node->data.assign.var);
            printAST(node->data.assign.value, level+1);
            break;
        case NODE_PRINT:
            printf("PRINT\n");
            printAST(node->data.expr, level+1);
            break;
        case NODE_STMT_LIST:
            printAST(node->data.stmtlist.stmt, level);
            printAST(node->data.stmtlist.next, level);
            break;
        case NODE_EXPR_LIST:
            printf("EXPR_LIST\n");
            printAST(node->data.exprlist.first, level+1);
            printAST(node->data.exprlist.next, level+1);
            break;
        case NODE_BREAK:
            printf("BREAK\n");
            break;
        case NODE_IF:
            printf("IF\n");
            printAST(node->data.if_stmt.cond, level+1);
            printAST(node->data.if_stmt.then_branch, level+1);
            if (node->data.if_stmt.else_branch)
                printAST(node->data.if_stmt.else_branch, level+1);
            break;
        case NODE_WHILE:
            printf("WHILE\n");
            printAST(node->data.while_stmt.cond, level+1);
            printAST(node->data.while_stmt.body, level+1);
            break;
        case NODE_FOR:
            printf("FOR\n");
            if (node->data.for_stmt.init) printAST(node->data.for_stmt.init, level+1);
            if (node->data.for_stmt.cond) printAST(node->data.for_stmt.cond, level+1);
            if (node->data.for_stmt.update) printAST(node->data.for_stmt.update, level+1);
            printAST(node->data.for_stmt.body, level+1);
            break;
        case NODE_RETURN:
            printf("RETURN\n");
            printAST(node->data.ret_expr, level+1);
            break;
        default:
            printf("Unknown node type %d\n", node->type);
    }
}
