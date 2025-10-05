#ifndef SYMTAB_H
#define SYMTAB_H

/* SYMBOL TABLE
 * Tracks all declared variables during compilation
 * Maps variable names to their memory locations (stack offsets)
 * Used for semantic checking and code generation
 */

/* Enhanced symtab.h */
#define HASH_SIZE 211  /* Prime number for better distribution */
#define MAX_VARS 1000  /* Increased capacity */

typedef struct SymbolNode {
    char* name;
    int offset;
    int isArray;
    int arraySize;
    struct SymbolNode* next;  /* For chaining */
} SymbolNode;

/* SYMBOL TABLE STRUCTURE */
typedef struct {
    SymbolNode* buckets[HASH_SIZE];
    int count;
    int nextOffset;
    /* Performance counters */
    int lookups;
    int collisions;
} SymbolTable;

/* Global symbol table instance (defined in symtab.c) */
extern SymbolTable symtab;

/* SYMBOL TABLE OPERATIONS */
void initSymTab();               /* Initialize empty symbol table */
int addVar(char* name);          /* Add new variable, returns offset or -1 if duplicate */
int addArray(char* name, int size); /* Add array variable, reserve size*4 bytes, return base offset or -1 */
int getVarOffset(char* name);    /* Get stack offset for variable, -1 if not found */
int isVarDeclared(char* name);   /* Check if variable exists (1=yes, 0=no) */
int isArray(char* name);         /* Check if variable is an array (1=yes, 0=no) */
int getArraySize(char* name);    /* Return array size (number of elements), -1 if not found/not array */
void freeSymTab();               /* Free all symbol table memory */

/* Hash function (djb2) */
unsigned int hash(const char* str);

#endif /* SYMTAB_H */