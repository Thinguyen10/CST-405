/* SYMBOL TABLE (hash table) IMPLEMENTATION */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* Global symbol table instance */
SymbolTable symtab;

/* djb2 hash function (string -> bucket index) */
unsigned int hash(const char* str) {
    unsigned long h = 5381;
    int c;
    while ((c = *str++))
        h = ((h << 5) + h) + c; /* h * 33 + c */
    return (unsigned int)(h % HASH_SIZE);
}

void initSymTab() {
    for (int i = 0; i < HASH_SIZE; i++) symtab.buckets[i] = NULL;
    symtab.count = 0;
    symtab.nextOffset = 0;
    symtab.lookups = 0;
    symtab.collisions = 0;
}

/* Internal: find node by name (returns node or NULL) */
static SymbolNode* findNode(const char* name) {
    unsigned int b = hash(name);
    SymbolNode* cur = symtab.buckets[b];
    while (cur) {
        symtab.lookups++;
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int isVarDeclared(char* name) {
    return findNode(name) != NULL;
}

int isArray(char* name) {
    SymbolNode* n = findNode(name);
    if (!n) return 0;
    return n->isArray;
}

int getArraySize(char* name) {
    SymbolNode* n = findNode(name);
    if (!n) return -1;
    return n->arraySize;
}

int getVarOffset(char* name) {
    SymbolNode* n = findNode(name);
    if (!n) return -1;
    return n->offset;
}

int addVar(char* name) {
    if (!name) return -1;
    if (isVarDeclared(name)) return -1;

    unsigned int b = hash(name);
    SymbolNode* node = malloc(sizeof(SymbolNode));
    node->name = strdup(name);
    node->offset = symtab.nextOffset;
    node->isArray = 0;
    node->arraySize = 0;
    node->next = symtab.buckets[b];

    if (symtab.buckets[b]) symtab.collisions++;
    symtab.buckets[b] = node;
    symtab.count++;
    symtab.nextOffset += 4; /* one word */
    return node->offset;
}

int addArray(char* name, int size) {
    if (!name || size <= 0) return -1;
    if (isVarDeclared(name)) return -1;

    unsigned int b = hash(name);
    SymbolNode* node = malloc(sizeof(SymbolNode));
    node->name = strdup(name);
    node->offset = symtab.nextOffset;
    node->isArray = 1;
    node->arraySize = size;
    node->next = symtab.buckets[b];

    if (symtab.buckets[b]) symtab.collisions++;
    symtab.buckets[b] = node;
    symtab.count++;
    symtab.nextOffset += size * 4; /* reserve size words */
    return node->offset;
}

/* Free entire symbol table */
void freeSymTab() {
    for (int i = 0; i < HASH_SIZE; i++) {
        SymbolNode* cur = symtab.buckets[i];
        while (cur) {
            SymbolNode* next = cur->next;
            free(cur->name);
            free(cur);
            cur = next;
        }
        symtab.buckets[i] = NULL;
    }
    symtab.count = 0;
    symtab.nextOffset = 0;
    symtab.lookups = 0;
    symtab.collisions = 0;
}