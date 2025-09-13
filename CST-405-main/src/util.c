/*
 * Utility Functions Implementation
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"
#include "globals.h"

/* Global error counters */
int error_count = 0;
int warning_count = 0;

/* Debug mode flag */
static int debug_mode = 0;

/* Copy a string */
char *copy_string(const char *s) {
    if (s == NULL) return NULL;
    
    char *copy = (char *)malloc(strlen(s) + 1);
    if (copy == NULL) {
        fatal_error("Out of memory copying string");
    }
    strcpy(copy, s);
    return copy;
}

/* Concatenate two strings */
char *concat_strings(const char *s1, const char *s2) {
    if (s1 == NULL) return copy_string(s2);
    if (s2 == NULL) return copy_string(s1);
    
    size_t len = strlen(s1) + strlen(s2) + 1;
    char *result = (char *)malloc(len);
    if (result == NULL) {
        fatal_error("Out of memory concatenating strings");
    }
    
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/* Compare strings for equality */
int string_equal(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) return 0;
    return strcmp(s1, s2) == 0;
}

/* Report an error */
void error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    error_count++;
}

/* Report a warning */
void warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    warning_count++;
}

/* Report a fatal error and exit */
void fatal_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "Fatal Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
    exit(1);
}

/* Safe malloc */
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL && size > 0) {
        fatal_error("Out of memory (malloc failed)");
    }
    return ptr;
}

/* Safe calloc */
void *safe_calloc(size_t count, size_t size) {
    void *ptr = calloc(count, size);
    if (ptr == NULL && count > 0 && size > 0) {
        fatal_error("Out of memory (calloc failed)");
    }
    return ptr;
}

/* Safe realloc */
void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL && size > 0) {
        fatal_error("Out of memory (realloc failed)");
    }
    return new_ptr;
}

/* Open a file */
FILE *open_file(const char *filename, const char *mode) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        fatal_error("Cannot open file '%s'", filename);
    }
    return file;
}

/* Close a file */
void close_file(FILE *file) {
    if (file != NULL && file != stdin && file != stdout && file != stderr) {
        fclose(file);
    }
}

/* Debug print */
void debug_print(const char *format, ...) {
    if (!debug_mode) return;
    
    va_list args;
    va_start(args, format);
    
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    
    va_end(args);
}

/* Set debug mode */
void set_debug_mode(int enable) {
    debug_mode = enable;
}