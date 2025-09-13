#ifndef UTIL_H
#define UTIL_H

/*
 * Utility Functions for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include <stdio.h>

/* String utilities */
char *copy_string(const char *s);
char *concat_strings(const char *s1, const char *s2);
int string_equal(const char *s1, const char *s2);

/* Error handling */
void error(const char *format, ...);
void warning(const char *format, ...);
void fatal_error(const char *format, ...);

/* Memory management */
void *safe_malloc(size_t size);
void *safe_calloc(size_t count, size_t size);
void *safe_realloc(void *ptr, size_t size);

/* File utilities */
FILE *open_file(const char *filename, const char *mode);
void close_file(FILE *file);

/* Debug utilities */
void debug_print(const char *format, ...);
void set_debug_mode(int enable);

#endif /* UTIL_H */