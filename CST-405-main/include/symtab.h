#ifndef SYMTAB_H
#define SYMTAB_H

/*
 * Symbol Table for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include "globals.h"
#include "ast.h"

/* Symbol kinds */
typedef enum {
    SYMBOL_VAR,
    SYMBOL_ARRAY,
    SYMBOL_FUNCTION,
    SYMBOL_PARAM
} SymbolKind;

/* Symbol table entry */
typedef struct SymbolEntry {
    char *name;                /* Symbol name */
    SymbolKind kind;           /* Kind of symbol */
    DataType type;             /* Data type */
    int scope_level;           /* Scope level (0 = global) */
    int memory_location;       /* Memory offset or address */
    int size;                  /* Size (for arrays) */
    int param_count;           /* Number of parameters (for functions) */
    struct SymbolEntry *params; /* Parameter list (for functions) */
    struct SymbolEntry *next;  /* Next entry in hash chain */
    int line_number;           /* Line where declared */
    int is_used;               /* Flag for unused variable warning */
} SymbolEntry;

/* Scope management */
typedef struct Scope {
    int level;
    int offset;                /* Current offset for local variables */
    struct Scope *parent;
    SymbolEntry **table;       /* Hash table for this scope */
} Scope;

/* Symbol table size */
#define SYMTAB_SIZE 211        /* Prime number for better hashing */

/* Global symbol table and scope */
extern Scope *current_scope;
extern Scope *global_scope;

/* Symbol table operations */
void init_symbol_table(void);
void enter_scope(void);
void exit_scope(void);
SymbolEntry *insert_symbol(char *name, SymbolKind kind, DataType type);
SymbolEntry *lookup_symbol(char *name);
SymbolEntry *lookup_symbol_in_scope(char *name, Scope *scope);
void set_symbol_type(SymbolEntry *symbol, DataType type);
void add_param_to_function(SymbolEntry *func, SymbolEntry *param);

/* Symbol table display */
void print_symbol_table(void);
void print_scope(Scope *scope);
void print_symbol(SymbolEntry *symbol);

/* Symbol table analysis */
void check_unused_symbols(void);
int get_symbol_count(void);
int get_scope_depth(void);

/* Memory management */
int allocate_memory(int size);
int get_current_offset(void);

/* Hash function */
int hash_function(char *name);

/* Cleanup */
void free_symbol_table(void);

#endif /* SYMTAB_H */