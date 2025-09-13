/*
 * Main Driver for C-Minus Compiler
 * CST-405 Compiler Design
 * 
 * This program demonstrates the complete compilation pipeline:
 * 1. Lexical Analysis (Flex)
 * 2. Syntax Analysis (Bison) 
 * 3. AST Construction
 * 4. Semantic Analysis
 * 5. Intermediate Code Generation (3-address code)
 * 6. Optimization
 * 7. Target Code Generation (MIPS)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "globals.h"
#include "ast.h"
#include "symtab.h"
#include "semantic.h"
#include "codegen.h"
#include "optimize.h"
#include "mips.h"
#include "util.h"

/* External declarations */
extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *ast_root;

/* Global file handles */
FILE *source_file = NULL;
FILE *output_file = NULL;
FILE *listing_file = NULL;

/* Compilation flags */
Boolean trace_scan = FALSE;
Boolean trace_parse = FALSE;
Boolean trace_semantic = FALSE;
Boolean trace_code = FALSE;
Boolean generate_code = TRUE;

/* Optimization level */
int optimization_level = 1;

/* Function prototypes */
void print_usage(const char *program_name);
void parse_arguments(int argc, char *argv[]);
void compile_file(const char *filename);

int main(int argc, char *argv[]) {
    printf("==============================================\n");
    printf("      C-MINUS COMPILER\n");
    printf("      CST-405 Compiler Design\n");
    printf("==============================================\n\n");
    
    /* Parse command line arguments */
    parse_arguments(argc, argv);
    
    /* Check if input file was provided */
    if (optind >= argc) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    /* Compile the input file */
    compile_file(argv[optind]);
    
    /* Report final status */
    printf("\n==============================================\n");
    printf("COMPILATION SUMMARY:\n");
    printf("  Errors:   %d\n", error_count);
    printf("  Warnings: %d\n", warning_count);
    
    if (error_count == 0) {
        printf("  Status:   SUCCESS\n");
    } else {
        printf("  Status:   FAILED\n");
    }
    printf("==============================================\n");
    
    return (error_count > 0) ? 1 : 0;
}

/* Compile a single file */
void compile_file(const char *filename) {
    printf("Compiling: %s\n\n", filename);
    
    /* Open source file */
    source_file = fopen(filename, "r");
    if (source_file == NULL) {
        fatal_error("Cannot open source file: %s", filename);
    }
    yyin = source_file;
    
    /* Create output filename */
    char output_filename[256];
    strcpy(output_filename, filename);
    char *ext = strrchr(output_filename, '.');
    if (ext) *ext = '\0';
    strcat(output_filename, ".s");
    
    /* Open output file */
    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        fatal_error("Cannot create output file: %s", output_filename);
    }
    
    /* Phase 1: Lexical and Syntax Analysis */
    printf("=== PHASE 1: PARSING ===\n");
    if (yyparse() != 0) {
        error("Parse failed");
        return;
    }
    
    if (ast_root == NULL) {
        error("No AST generated");
        return;
    }
    
    /* Display AST if requested */
    if (trace_parse) {
        printf("\n=== ABSTRACT SYNTAX TREE ===\n");
        print_ast(ast_root, 0);
        
        /* Also show parse tree vs AST comparison */
        show_parse_tree_vs_ast();
    }
    
    /* Phase 2: Semantic Analysis */
    printf("\n=== PHASE 2: SEMANTIC ANALYSIS ===\n");
    semantic_analysis(ast_root);
    
    if (error_count > 0) {
        printf("Compilation terminated due to semantic errors.\n");
        return;
    }
    
    /* Phase 3: Intermediate Code Generation */
    if (generate_code) {
        printf("\n=== PHASE 3: INTERMEDIATE CODE GENERATION ===\n");
        generate_tac(ast_root);
        
        /* Phase 4: Optimization */
        if (optimization_level > 0) {
            printf("\n=== PHASE 4: OPTIMIZATION ===\n");
            optimize_tac(optimization_level);
            
            if (trace_code) {
                printf("\n=== OPTIMIZED THREE-ADDRESS CODE ===\n");
                print_tac();
            }
        }
        
        /* Phase 5: Target Code Generation */
        printf("\n=== PHASE 5: TARGET CODE GENERATION ===\n");
        generate_mips(get_tac_list(), output_file);
        printf("Output written to: %s\n", output_filename);
    }
    
    /* Clean up */
    fclose(source_file);
    fclose(output_file);
    free_ast(ast_root);
}

/* Parse command line arguments */
void parse_arguments(int argc, char *argv[]) {
    int opt;
    
    static struct option long_options[] = {
        {"help",        no_argument,       0, 'h'},
        {"trace-scan",  no_argument,       0, 's'},
        {"trace-parse", no_argument,       0, 'p'},
        {"trace-sem",   no_argument,       0, 'a'},
        {"trace-code",  no_argument,       0, 'c'},
        {"optimize",    required_argument, 0, 'O'},
        {"no-code",     no_argument,       0, 'n'},
        {"output",      required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "hspacO:no:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                exit(0);
                
            case 's':
                trace_scan = TRUE;
                printf("Scanner tracing enabled\n");
                break;
                
            case 'p':
                trace_parse = TRUE;
                printf("Parser tracing enabled\n");
                break;
                
            case 'a':
                trace_semantic = TRUE;
                printf("Semantic analysis tracing enabled\n");
                break;
                
            case 'c':
                trace_code = TRUE;
                printf("Code generation tracing enabled\n");
                break;
                
            case 'O':
                optimization_level = atoi(optarg);
                printf("Optimization level: %d\n", optimization_level);
                break;
                
            case 'n':
                generate_code = FALSE;
                printf("Code generation disabled\n");
                break;
                
            case 'o':
                /* Output file handling would go here */
                break;
                
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }
}

/* Print usage information */
void print_usage(const char *program_name) {
    printf("\nUsage: %s [options] source_file.cm\n", program_name);
    printf("\nOptions:\n");
    printf("  -h, --help         Show this help message\n");
    printf("  -s, --trace-scan   Enable scanner tracing\n");
    printf("  -p, --trace-parse  Enable parser tracing and show AST\n");
    printf("  -a, --trace-sem    Enable semantic analysis tracing\n");
    printf("  -c, --trace-code   Enable code generation tracing\n");
    printf("  -O<level>          Set optimization level (0-2)\n");
    printf("  -n, --no-code      Disable code generation\n");
    printf("  -o <file>          Specify output file\n");
    printf("\nExample:\n");
    printf("  %s -p -O2 test.cm    # Parse with AST display and optimize\n", program_name);
    printf("  %s -spacO2 test.cm   # Enable all tracing with optimization\n", program_name);
    printf("\n");
}