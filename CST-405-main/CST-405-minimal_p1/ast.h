#ifndef AST_H
#define AST_H

/* ABSTRACT SYNTAX TREE (AST)
 * The AST is an intermediate representation of the program structure
 * It represents the hierarchical syntax of the source code
 * Each node represents a construct in the language
 */

/* NODE TYPES - Different kinds of AST nodes in our language */
typedef enum {
    NODE_NUM,       /* Numeric literal (e.g., 42) */
    NODE_VAR,       /* Variable reference (e.g., x) */
    NODE_BINOP,     /* Binary operation (e.g., x + y) */
    NODE_DECL,      /* Variable declaration (e.g., int x) */
    NODE_EXPR_LIST, /* List of expressions (e.g., function arguments) */
    NODE_ASSIGN,    /* Assignment statement (e.g., x = 10) */
    NODE_PRINT,     /* Print statement (e.g., print(x)) */
    NODE_STMT_LIST, /* List of statements (program structure) */
    NODE_UNARY,     /* Unary operations (e.g., -x) */
    NODE_BREAK,     /* Break statement */
    NODE_IF,        /* If statement */
    NODE_WHILE,     /* While loop */
    NODE_FOR,       /* For loop */
    NODE_RETURN,     /* Return statement */
    NODE_ARRAY_DECL, /* Array declaration */
    NODE_ARRAY_ACCESS, /* Array access */
    NODE_ARRAY_ASSIGN /* Array assignment */
} NodeType;

/* AST NODE STRUCTURE
 * Uses a union to efficiently store different node data
 * Only the relevant fields for each node type are used
 */
typedef struct ASTNode {
    NodeType type;  /* Identifies what kind of node this is */
    
    union {
        /* Literal number value (NODE_NUM) */
        double num;  /* matches lexer returning double */
        
        /* Variable name (NODE_VAR) */
        char* name;
        
        /* Binary operation structure (NODE_BINOP) */
        struct {
            char* op;                 /* Operator string ("+", "==", etc.) */
            struct ASTNode* left;     /* Left operand */
            struct ASTNode* right;    /* Right operand */
        } binop;

        /* Unary operation structure (NODE_UNARY) */
        struct {
            char* op;                 /* Operator string ("-", "!") */
            struct ASTNode* expr;     /* Operand */
        } unary;

        /* Declaration structure (NODE_DECL) */
        struct {
            char* name;               /* Variable name */
            struct ASTNode* init;     /* Optional initializer expression */
        } decl;

        /* Expression list structure (NODE_EXPR_LIST) */
        struct {
            struct ASTNode* first;    /* First expression */
            struct ASTNode* next;     /* Rest of list */
        } exprlist;

        /* Assignment structure (NODE_ASSIGN) */
        struct {
            char* var;                /* Variable being assigned to */
            struct ASTNode* value;    /* Expression being assigned */
        } assign;

        /* Print expression (NODE_PRINT) */
        struct ASTNode* expr;

        /* Statement list structure (NODE_STMT_LIST) */
        struct {
            struct ASTNode* stmt;     /* Current statement */
            struct ASTNode* next;     /* Rest of the list */
        } stmtlist;

        /* If statement structure (NODE_IF) */
        struct {
            struct ASTNode* cond;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;

        /* While statement structure (NODE_WHILE) */
        struct {
            struct ASTNode* cond;
            struct ASTNode* body;
        } while_stmt;

        /* For statement structure (NODE_FOR) */
        struct {
            struct ASTNode* init;
            struct ASTNode* cond;
            struct ASTNode* update;
            struct ASTNode* body;
        } for_stmt;
        /* Array declaration structure (NODE_ARRAY_DECL) */
        struct {
            char* name;               /* Array name */
            struct ASTNode* size;     /* Array size expression */
            struct ASTNode* init;     /* Optional initializer list (expr_list) */
        } array_decl;

        /* Array access structure (NODE_ARRAY_ACCESS) */
        struct {
            char* name;               /* Array name */
            struct ASTNode* index;    /* Index expression */
        } array_access;

        /* Array assignment structure (NODE_ARRAY_ASSIGN) */
        struct {
            char* name;               /* Array name */
            struct ASTNode* index;    /* Index expression */
            struct ASTNode* value;    /* Value expression */
        } array_assign;
        /* Return statement structure (NODE_RETURN) */
        struct ASTNode* ret_expr;

    } data;
} ASTNode;

/* AST CONSTRUCTION FUNCTIONS */
ASTNode* createNum(double value);
ASTNode* createVar(char* name);
ASTNode* createBinOp(const char* op, ASTNode* left, ASTNode* right);
ASTNode* createUnaryOp(const char* op, ASTNode* expr);
ASTNode* createDecl(char* name, ASTNode* init);
ASTNode* createAssign(char* var, ASTNode* value);
ASTNode* createPrint(ASTNode* expr);
/* Array-related constructors (name, size, optional initializer/list) */
ASTNode* createArrayDecl(char* name, ASTNode* size, ASTNode* initList);
ASTNode* createArrayAccess(char* name, ASTNode* index);
ASTNode* createArrayAssign(char* name, ASTNode* index, ASTNode* value);
ASTNode* createStmtList(ASTNode* stmt1, ASTNode* stmt2);
ASTNode* createExprList(ASTNode* first, ASTNode* rest);
ASTNode* addToExprList(ASTNode* list, ASTNode* expr);
ASTNode* createBreak();
ASTNode* createIf(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* createWhile(ASTNode* cond, ASTNode* body);
ASTNode* createFor(ASTNode* init, ASTNode* cond, ASTNode* update, ASTNode* body);
ASTNode* createReturn(ASTNode* expr);

/* AST DISPLAY FUNCTION */
void printAST(ASTNode* node, int level);

#endif
