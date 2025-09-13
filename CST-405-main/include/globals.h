#ifndef GLOBALS_H
#define GLOBALS_H

/*
 * Global definitions for C-Minus Compiler
 * CST-405 Compiler Design
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Boolean type definition */
typedef enum { FALSE, TRUE } Boolean;

/* Maximum sizes */
#define MAX_ID_LENGTH 256
#define MAX_STRING_LENGTH 1024

/* Error handling */
extern int error_count;
extern int warning_count;

/* File handles */
extern FILE *source_file;
extern FILE *output_file;
extern FILE *listing_file;

/* Compilation flags */
extern Boolean trace_scan;
extern Boolean trace_parse;
extern Boolean trace_semantic;
extern Boolean trace_code;
extern Boolean generate_code;

/* Current line and column numbers */
extern int linenum;
extern int colnum;

/* Utility functions */
void error(const char *message, ...);
void warning(const char *message, ...);
void fatal_error(const char *message, ...);
char *copy_string(const char *s);

#endif /* GLOBALS_H */