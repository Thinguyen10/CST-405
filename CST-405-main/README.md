# C-Minus Compiler
## CST-405 Compiler Design Course

A complete compiler implementation for the C-Minus language, demonstrating all phases of compilation from source code to MIPS assembly.

## Overview

This project implements a full compiler for C-Minus, a simplified subset of C, using traditional compiler construction tools (Flex and Bison) along with hand-written components for semantic analysis, code generation, and optimization.

### Compilation Phases

1. **Lexical Analysis** (Flex) - Tokenizes the source code
2. **Syntax Analysis** (Bison) - Parses tokens and builds parse tree
3. **AST Construction** - Builds abstract syntax tree
4. **Semantic Analysis** - Type checking and symbol table management
5. **Intermediate Code Generation** - Generates three-address code
6. **Optimization** - Performs various code optimizations
7. **Target Code Generation** - Produces MIPS assembly code

## Project Structure

```
cminus-compiler/
├── src/                 # Source files
│   ├── lexer.l         # Flex lexer specification
│   ├── parser.y        # Bison parser specification
│   ├── ast.c           # AST implementation
│   ├── symtab.c        # Symbol table
│   ├── semantic.c      # Semantic analyzer
│   ├── codegen.c       # 3-address code generator
│   ├── optimize.c      # Optimizer
│   ├── mips.c          # MIPS code generator
│   ├── main.c          # Main driver
│   └── util.c          # Utility functions
├── include/            # Header files
│   ├── globals.h       # Global definitions
│   ├── ast.h           # AST declarations
│   ├── symtab.h        # Symbol table declarations
│   ├── semantic.h      # Semantic analyzer declarations
│   ├── codegen.h       # Code generation declarations
│   ├── optimize.h      # Optimizer declarations
│   ├── mips.h          # MIPS generator declarations
│   └── util.h          # Utility declarations
├── tests/              # Sample C-Minus programs
│   ├── simple.cm       # Basic arithmetic
│   ├── factorial.cm    # Recursive factorial
│   ├── fibonacci.cm    # Fibonacci sequence
│   ├── gcd.cm          # Greatest common divisor
│   └── sort.cm         # Bubble sort
├── docs/               # Documentation
│   └── grammar.txt     # C-Minus grammar specification
├── Makefile            # Build configuration
└── README.md           # This file
```

## Building the Compiler

### Prerequisites

- GCC compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make build tool

### Build Instructions

```bash
# Build the compiler
make

# Clean build artifacts
make clean

# Run tests
make test
```

## Using the Compiler

### Basic Usage

```bash
# Compile a C-Minus program
./cminus source.cm

# This generates source.s (MIPS assembly)
```

### Command Line Options

```bash
./cminus [options] source_file.cm

Options:
  -h, --help         Show help message
  -s, --trace-scan   Enable scanner tracing
  -p, --trace-parse  Enable parser tracing and show AST
  -a, --trace-sem    Enable semantic analysis tracing
  -c, --trace-code   Enable code generation tracing
  -O<level>          Set optimization level (0-2)
  -n, --no-code      Disable code generation
  -o <file>          Specify output file

Examples:
  ./cminus -p test.cm        # Show AST
  ./cminus -O2 test.cm       # Optimize level 2
  ./cminus -spac test.cm     # Enable all tracing
```

## C-Minus Language Features

### Data Types
- `int` - Integer type
- `void` - Void type (for functions)
- Arrays of integers

### Statements
- Variable declarations
- Function declarations
- Assignment statements
- If-else statements
- While loops
- Return statements
- Compound statements (blocks)

### Expressions
- Arithmetic: `+`, `-`, `*`, `/`
- Relational: `<`, `<=`, `>`, `>=`, `==`, `!=`
- Function calls
- Array indexing

### Built-in Functions
- `input()` - Read integer from user
- `output(int)` - Print integer to console

## Example Programs

### Factorial (Recursive)
```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

void main(void) {
    int num;
    num = input();
    output(factorial(num));
}
```

### Fibonacci Sequence
```c
void main(void) {
    int n;
    int fib[20];
    int i;
    
    n = input();
    fib[0] = 0;
    fib[1] = 1;
    
    i = 2;
    while (i < n) {
        fib[i] = fib[i-1] + fib[i-2];
        i = i + 1;
    }
    
    i = 0;
    while (i < n) {
        output(fib[i]);
        i = i + 1;
    }
}
```

## Compiler Internals

### Abstract Syntax Tree (AST)

The compiler builds an AST from the parse tree, removing syntactic details and focusing on semantic structure. This makes subsequent analysis and code generation more efficient.

**Parse Tree vs AST Example:**
```
Expression: x = 2 + 3 * 4

Parse Tree (detailed):              AST (simplified):
     assignment                          ASSIGN
     /    |    \                         /    \
   var   '='  expr                    ID:x     +
    |           |                             /   \
   ID:x    add-expr                        NUM:2   *
           /   |   \                              /   \
        term  '+'  term                       NUM:3  NUM:4
         |          |
       NUM:2   term * factor
                |       |
              NUM:3   NUM:4
```

### Symbol Table

Hierarchical scope-based symbol table with support for:
- Global and local scopes
- Function parameters
- Array declarations
- Type checking
- Memory allocation tracking

### Three-Address Code

Intermediate representation using simple instructions:
```
t0 = 3 * 4
t1 = 2 + t0
x = t1
```

### Optimization Techniques

1. **Constant Folding** - Evaluate constant expressions at compile time
2. **Constant Propagation** - Replace variables with known constants
3. **Dead Code Elimination** - Remove unreachable or unused code
4. **Copy Propagation** - Replace copies with original values
5. **Algebraic Simplification** - Simplify expressions (x+0 → x, x*1 → x)
6. **Common Subexpression Elimination** - Reuse computed values

### MIPS Code Generation

Generates MIPS assembly code with:
- Register allocation (using $t0-$t7, $s0-$s7)
- Stack frame management
- Function calling conventions
- System calls for I/O

## Educational Value

This compiler demonstrates:

1. **Lexical Analysis** - Pattern matching with regular expressions
2. **Parsing** - Context-free grammars and LALR parsing
3. **AST Construction** - Tree data structures and traversal
4. **Symbol Tables** - Hash tables and scope management
5. **Type Checking** - Static semantic analysis
6. **Code Generation** - Instruction selection and register allocation
7. **Optimization** - Data flow analysis and transformation
8. **Target Architecture** - MIPS assembly and calling conventions

## Testing

Run the test suite:
```bash
make test
```

Individual test programs:
```bash
./cminus tests/factorial.cm
./cminus tests/fibonacci.cm
./cminus tests/sort.cm
```

## Limitations

- No floating-point support
- No string support
- No dynamic memory allocation
- Limited to 32-bit integers
- No preprocessor directives
- No separate compilation/linking

## Future Enhancements

- Add support for more data types (char, float)
- Implement more optimization passes
- Add register allocation with graph coloring
- Support for multiple source files
- Generate x86 or ARM assembly
- Add debugging information
- Implement error recovery

## References

- Aho, Sethi, Ullman - "Compilers: Principles, Techniques, and Tools" (Dragon Book)
- Flex & Bison documentation
- MIPS assembly language reference
- C-Minus language specification

## Author

Created for CST-405 Compiler Design Course

## License

Educational use only