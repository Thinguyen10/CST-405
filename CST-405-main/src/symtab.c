/*
 * Symbol Table Implementation
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

/* Global variables */
Scope *current_scope = NULL;
Scope *global_scope = NULL;
static int global_offset = 0;

/* Hash function for symbol table */
int hash_function(char *name) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash << 4) + *name++;
        unsigned int g = hash & 0xf0000000;
        if (g) {
            hash ^= g >> 24;
            hash &= ~g;
        }
    }
    return hash % SYMTAB_SIZE;
}

/* Initialize symbol table */
void init_symbol_table(void) {
    global_scope = (Scope *)malloc(sizeof(Scope));
    global_scope->level = 0;
    global_scope->offset = 0;
    global_scope->parent = NULL;
    global_scope->table = (SymbolEntry **)calloc(SYMTAB_SIZE, sizeof(SymbolEntry *));
    current_scope = global_scope;
    
    /* Insert built-in functions */
    SymbolEntry *input_func = insert_symbol("input", SYMBOL_FUNCTION, TYPE_FUNCTION);
    input_func->param_count = 0;
    
    SymbolEntry *output_func = insert_symbol("output", SYMBOL_FUNCTION, TYPE_FUNCTION);
    output_func->param_count = 1;
    SymbolEntry *output_param = (SymbolEntry *)malloc(sizeof(SymbolEntry));
    output_param->name = copy_string("value");
    output_param->kind = SYMBOL_PARAM;
    output_param->type = TYPE_INT;
    output_param->next = NULL;
    output_func->params = output_param;
}

/* Enter a new scope */
void enter_scope(void) {
    Scope *new_scope = (Scope *)malloc(sizeof(Scope));
    new_scope->level = current_scope->level + 1;
    new_scope->offset = 0;
    new_scope->parent = current_scope;
    new_scope->table = (SymbolEntry **)calloc(SYMTAB_SIZE, sizeof(SymbolEntry *));
    current_scope = new_scope;
}

/* Exit current scope */
void exit_scope(void) {
    if (current_scope != global_scope) {
        Scope *old_scope = current_scope;
        current_scope = current_scope->parent;
        
        /* Check for unused symbols before freeing */
        for (int i = 0; i < SYMTAB_SIZE; i++) {
            SymbolEntry *entry = old_scope->table[i];
            while (entry) {
                if (!entry->is_used && entry->kind != SYMBOL_FUNCTION) {
                    warning("Variable '%s' declared but never used (line %d)", 
                            entry->name, entry->line_number);
                }
                entry = entry->next;
            }
        }
        
        /* Note: In a real compiler, we might want to keep the scope
           for later phases, but for demonstration we'll free it */
        free(old_scope->table);
        free(old_scope);
    }
}

/* Insert a symbol into the current scope */
SymbolEntry *insert_symbol(char *name, SymbolKind kind, DataType type) {
    int index = hash_function(name);
    
    /* Check if symbol already exists in current scope */
    SymbolEntry *entry = current_scope->table[index];
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            error("Symbol '%s' already declared in this scope", name);
            return NULL;
        }
        entry = entry->next;
    }
    
    /* Create new symbol entry */
    SymbolEntry *new_entry = (SymbolEntry *)malloc(sizeof(SymbolEntry));
    new_entry->name = copy_string(name);
    new_entry->kind = kind;
    new_entry->type = type;
    new_entry->scope_level = current_scope->level;
    new_entry->line_number = linenum;
    new_entry->is_used = 0;
    new_entry->params = NULL;
    new_entry->param_count = 0;
    
    /* Allocate memory based on kind and scope */
    if (kind == SYMBOL_VAR || kind == SYMBOL_PARAM) {
        new_entry->size = 1;
        if (current_scope->level == 0) {
            new_entry->memory_location = global_offset;
            global_offset += 4;  /* 4 bytes for int */
        } else {
            new_entry->memory_location = current_scope->offset;
            current_scope->offset += 4;
        }
    } else if (kind == SYMBOL_ARRAY) {
        /* Size will be set separately */
        new_entry->size = 0;
        new_entry->memory_location = current_scope->offset;
    } else if (kind == SYMBOL_FUNCTION) {
        new_entry->memory_location = -1;  /* Functions don't have memory location */
    }
    
    /* Insert at beginning of hash chain */
    new_entry->next = current_scope->table[index];
    current_scope->table[index] = new_entry;
    
    return new_entry;
}

/* Lookup a symbol in all visible scopes */
SymbolEntry *lookup_symbol(char *name) {
    Scope *scope = current_scope;
    
    while (scope) {
        SymbolEntry *entry = lookup_symbol_in_scope(name, scope);
        if (entry) {
            entry->is_used = 1;
            return entry;
        }
        scope = scope->parent;
    }
    
    return NULL;
}

/* Lookup a symbol in a specific scope */
SymbolEntry *lookup_symbol_in_scope(char *name, Scope *scope) {
    int index = hash_function(name);
    SymbolEntry *entry = scope->table[index];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }
    
    return NULL;
}

/* Set the type of a symbol */
void set_symbol_type(SymbolEntry *symbol, DataType type) {
    if (symbol) {
        symbol->type = type;
    }
}

/* Add a parameter to a function */
void add_param_to_function(SymbolEntry *func, SymbolEntry *param) {
    if (func && func->kind == SYMBOL_FUNCTION) {
        if (func->params == NULL) {
            func->params = param;
        } else {
            SymbolEntry *last = func->params;
            while (last->next) {
                last = last->next;
            }
            last->next = param;
        }
        func->param_count++;
    }
}

/* Print entire symbol table */
void print_symbol_table(void) {
    printf("\n=== SYMBOL TABLE ===\n");
    printf("%-15s %-10s %-10s %-8s %-8s %-8s\n", 
           "Name", "Kind", "Type", "Scope", "Memory", "Line");
    printf("----------------------------------------------------------------\n");
    
    Scope *scope = current_scope;
    while (scope) {
        print_scope(scope);
        scope = scope->parent;
    }
    printf("\n");
}

/* Print a specific scope */
void print_scope(Scope *scope) {
    for (int i = 0; i < SYMTAB_SIZE; i++) {
        SymbolEntry *entry = scope->table[i];
        while (entry) {
            print_symbol(entry);
            entry = entry->next;
        }
    }
}

/* Print a single symbol */
void print_symbol(SymbolEntry *symbol) {
    char *kind_str;
    switch (symbol->kind) {
        case SYMBOL_VAR: kind_str = "Variable"; break;
        case SYMBOL_ARRAY: kind_str = "Array"; break;
        case SYMBOL_FUNCTION: kind_str = "Function"; break;
        case SYMBOL_PARAM: kind_str = "Parameter"; break;
        default: kind_str = "Unknown"; break;
    }
    
    char *type_str;
    switch (symbol->type) {
        case TYPE_INT: type_str = "int"; break;
        case TYPE_VOID: type_str = "void"; break;
        case TYPE_ARRAY: type_str = "int[]"; break;
        case TYPE_FUNCTION: type_str = "function"; break;
        default: type_str = "error"; break;
    }
    
    printf("%-15s %-10s %-10s %-8d %-8d %-8d", 
           symbol->name, kind_str, type_str,
           symbol->scope_level, symbol->memory_location, symbol->line_number);
    
    if (symbol->kind == SYMBOL_FUNCTION) {
        printf(" [%d params]", symbol->param_count);
    } else if (symbol->kind == SYMBOL_ARRAY) {
        printf(" [size: %d]", symbol->size);
    }
    
    printf("\n");
}

/* Check for unused symbols */
void check_unused_symbols(void) {
    Scope *scope = current_scope;
    
    while (scope) {
        for (int i = 0; i < SYMTAB_SIZE; i++) {
            SymbolEntry *entry = scope->table[i];
            while (entry) {
                if (!entry->is_used && 
                    entry->kind != SYMBOL_FUNCTION &&
                    strcmp(entry->name, "main") != 0) {
                    warning("Symbol '%s' declared but never used (line %d)",
                            entry->name, entry->line_number);
                }
                entry = entry->next;
            }
        }
        scope = scope->parent;
    }
}

/* Get total symbol count */
int get_symbol_count(void) {
    int count = 0;
    Scope *scope = current_scope;
    
    while (scope) {
        for (int i = 0; i < SYMTAB_SIZE; i++) {
            SymbolEntry *entry = scope->table[i];
            while (entry) {
                count++;
                entry = entry->next;
            }
        }
        scope = scope->parent;
    }
    
    return count;
}

/* Get current scope depth */
int get_scope_depth(void) {
    return current_scope ? current_scope->level : -1;
}

/* Allocate memory */
int allocate_memory(int size) {
    int location = current_scope->offset;
    current_scope->offset += size;
    return location;
}

/* Get current offset */
int get_current_offset(void) {
    return current_scope->offset;
}

/* Free symbol table */
void free_symbol_table(void) {
    /* In a real implementation, we would properly free all memory */
    /* For now, this is a placeholder */
}