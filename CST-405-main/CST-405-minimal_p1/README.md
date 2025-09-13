# Minimal C Compiler - Educational Version

A fully functional compiler demonstrating all phases of compilation with extensive educational features. Perfect for teaching compiler design concepts with real, working code.

## 🎯 Purpose

This compiler strips away complexity to show the **essential components** of compilation:
- **Minimal Language**: Just enough features to demonstrate key concepts
- **Clear Phases**: Each compilation phase is visible and well-documented
- **Real Output**: Generates actual MIPS assembly that runs on simulators
- **Educational Focus**: Extensive comments and explanatory output

## 📚 Language Features

Our minimal C-like language supports:
- **Integer variable declarations**: `int x;`
- **Assignment statements**: `x = 10;`
- **Addition operator**: `x + y`
- **Print statement**: `print(x);`

That's it! No loops, no conditions, no functions - just the bare essentials.

## 🔧 Compiler Architecture

### Complete Compilation Pipeline

```
Source Code (.c)
      ↓
┌─────────────────┐
│ LEXICAL ANALYSIS│ → Tokens (INT, ID, NUM, +, =, etc.)
│   (scanner.l)   │
└─────────────────┘
      ↓
┌─────────────────┐
│ SYNTAX ANALYSIS │ → Abstract Syntax Tree (AST)
│   (parser.y)    │
└─────────────────┘
      ↓
┌─────────────────┐
│  AST BUILDING   │ → Hierarchical program structure
│    (ast.c)      │
└─────────────────┘
      ↓
┌─────────────────┐
│ SEMANTIC CHECK  │ → Symbol table, type checking
│   (symtab.c)    │
└─────────────────┘
      ↓
┌─────────────────┐
│ INTERMEDIATE    │ → Three-Address Code (TAC)
│ CODE (tac.c)    │
└─────────────────┘
      ↓
┌─────────────────┐
│  OPTIMIZATION   │ → Constant folding, propagation
│   (tac.c)       │
└─────────────────┘
      ↓
┌─────────────────┐
│ CODE GENERATION │ → MIPS Assembly (.s)
│  (codegen.c)    │
└─────────────────┘
      ↓
MIPS Assembly (.s)
```

## 💾 Understanding the Stack

### What is the Stack?

The stack is a **real region of memory** that every program uses for storing local variables. It's not just an educational concept - it's how actual computers work!

### Stack Memory Layout

```
High Memory Address (0xFFFFFFFF)
         ↑
    ┌──────────┐
    │          │
    │  UNUSED  │
    │          │
    ├──────────┤
    │          │
    │  STACK   │ ← Your variables live here!
    │    ↓     │   (grows downward)
    │          │
$sp →├──────────┤ ← Stack Pointer points to top
    │          │
    │   FREE   │
    │  SPACE   │
    │          │
    ├──────────┤
    │    ↑     │
    │   HEAP   │ ← Dynamic memory (malloc)
    │          │   (grows upward)
    ├──────────┤
    │  GLOBALS │ ← Global variables
    ├──────────┤
    │   CODE   │ ← Your program
    └──────────┘
         ↓
Low Memory Address (0x00000000)
```

### How Our Compiler Uses the Stack

For this program:
```c
int x;
int y; 
int z;
x = 10;
y = 20;
z = x + y;
```

The stack layout becomes:

```
        Stack Memory
    ┌─────────────────┐
    │      ...        │
    ├─────────────────┤ ← Old $sp (before allocation)
    │   z (offset 8)  │ → Will store 30
    ├─────────────────┤
    │   y (offset 4)  │ → Stores 20
    ├─────────────────┤
    │   x (offset 0)  │ → Stores 10
    └─────────────────┘ ← $sp after "addi $sp, $sp, -400"
```

### MIPS Instructions for Stack Operations

```mips
# Allocate space (at program start)
addi $sp, $sp, -400    # Move stack pointer down 400 bytes

# Store value 10 in variable x (offset 0)
li $t0, 10            # Load immediate 10 into register $t0
sw $t0, 0($sp)        # Store Word: memory[$sp + 0] = $t0

# Load variable x into register
lw $t1, 0($sp)        # Load Word: $t1 = memory[$sp + 0]

# Deallocate space (at program end)
addi $sp, $sp, 400    # Restore stack pointer
```

## 🚀 Build & Run

### Prerequisites
- `flex` (lexical analyzer generator)
- `bison` (parser generator)
- `gcc` (C compiler)
- MIPS simulator (MARS, SPIM, or QtSPIM) for running output

### Compilation
```bash
# Build the compiler
make

# Compile a source file
./minicompiler test.c output.s

# Clean build files
make clean
```

### Example Session
```bash
$ ./minicompiler test.c output.s

╔════════════════════════════════════════════════════════════╗
║          MINIMAL C COMPILER - EDUCATIONAL VERSION         ║
╚════════════════════════════════════════════════════════════╝

┌──────────────────────────────────────────────────────────┐
│ PHASE 1: LEXICAL & SYNTAX ANALYSIS                       │
├──────────────────────────────────────────────────────────┤
│ • Reading source file: test.c
│ • Tokenizing input (scanner.l)
│ • Parsing grammar rules (parser.y)
│ • Building Abstract Syntax Tree
└──────────────────────────────────────────────────────────┘
✓ Parse successful - program is syntactically correct!

[... followed by AST, TAC, optimizations, and MIPS generation ...]
```

## 📝 Example Programs

### Simple Addition
```c
int a;
int b;
int sum;
a = 5;
b = 7;
sum = a + b;
print(sum);    // Output: 12
```

### Multiple Operations
```c
int x;
int y;
int z;
x = 10;
y = 20;
z = x + y;     // z = 30
print(z);
x = z + 5;     // x = 35
print(x);
```

## 🎓 Educational Features

### 1. **Extensive Comments**
Every source file contains detailed explanations of:
- What each component does
- Why design decisions were made
- How pieces fit together

### 2. **Visual Output**
The compiler shows:
- Beautiful ASCII boxes for each phase
- Line-by-line TAC with explanations
- Optimization steps clearly marked
- Success/error messages with helpful tips

### 3. **Phase Separation**
Each compilation phase is clearly separated:
- Lexical Analysis → Tokens
- Syntax Analysis → AST
- Intermediate Code → TAC
- Optimization → Improved TAC
- Code Generation → MIPS

### 4. **Real-World Concepts**
Students learn:
- How variables are stored in memory
- What three-address code looks like
- How optimizations work (constant folding)
- How high-level code becomes assembly

## 📁 File Structure

```
CST-405-minimal/
├── scanner.l      # Lexical analyzer (tokenizer)
├── parser.y       # Grammar rules and parser
├── ast.h/c        # Abstract Syntax Tree
├── symtab.h/c     # Symbol table for variables
├── tac.h/c        # Three-address code generation
├── codegen.h/c    # MIPS code generator
├── main.c         # Driver program
├── Makefile       # Build configuration
├── test.c         # Example program
└── README.md      # This file
```

## 🔍 Understanding the Output

### Three-Address Code (TAC)
```
1: DECL x          // Declare variable 'x'
2: x = 10          // Assign value to x
3: t0 = x + y      // Add: store result in t0
4: z = t0          // Assign temp result to z
5: PRINT z         // Output value of z
```

### Optimized TAC
```
1: DECL x
2: x = 10          // Constant value: 10
3: t0 = 30         // Folded: 10 + 20 = 30
4: z = 30          // Propagated constant
5: PRINT 30        // Direct constant print
```

## 🎯 Learning Objectives

Students will understand:
1. **Lexical Analysis**: How text becomes tokens
2. **Parsing**: How tokens become syntax trees
3. **Semantic Analysis**: How meaning is verified
4. **Intermediate Representations**: Platform-independent code
5. **Optimization**: Improving code efficiency
6. **Code Generation**: Creating real machine code
7. **Memory Management**: How variables are stored
8. **Compilation Pipeline**: How phases connect

## 🤝 Contributing

This is an educational project. Suggestions for making concepts clearer are welcome!

## 📜 License

Educational use - free to use and modify for teaching purposes.