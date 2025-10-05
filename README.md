# Compiler Project Progress Report

This repository documents the development of a custom compiler built using **Flex**, **Bison**, and **C**.  
It covers the full compilation pipeline — from **lexical analysis** and **parsing**, to **AST construction**, **TAC generation/optimization**, and **MIPS assembly output**.

---

## Week 1 — Enhanced `lexer.l` Rules

### Enhancements
Improved the **Lexer (`scanner.l`)** to recognize a richer subset of tokens and language constructs:
- Added additional **data types** beyond `int`
- Added **keywords**: `if`, `else`, `for`, `while`, `return`, `break`
- Added **operators**: `==`, `<`, `>`, `&&`, `/`, `*`
- Added **brackets/separators**: `{}`, `[]`
- Recognized both **integers** and **floats**
- Introduced a dedicated `PRINT` token

```lex
"print" {
    printf("%s : PRINT\n", yytext);
    return PRINT;
}
````

### File Updates

* **parser.y** — Added new token declarations to match lexer improvements.
* **Rebuild Commands:**

  ```bash
  bison -d parser.y
  flex improved_scanner.l
  ```

---

## Week 2 — Parser & AST Integration

### `ast.h`

* Added `NODE_EXPR_LIST` to `NodeType` enum to represent multiple expressions.
* Added an `exprlist` struct to the AST union.
* Updated function prototypes:

  * `createBinOp`: changed from `char*` → `const char*` to support multi-character operators (`==`, `!=`, `>=`, etc.)
  * `createDecl`: added `init` to handle initialization (`int x = 5;`)

### `ast.c`

* Updated `createDecl` to store variable name in `.decl.name` and handle initialization.
* Added full support for `NODE_EXPR_LIST`.
* Updated `printAST()` to print `NODE_DECL.init` and expression lists.
* Fixed `createBinOp` to handle operator strings safely with `const char*`.

### `parser.y`

* Added typed rules for **for-loops**:

  ```yacc
  %type <node> for_init for_cond for_update
  ```
* Updated binary operation rules to use string operators.
* Added rules for expression lists:

  ```yacc
  expr_list: expr | expr_list ',' expr
  ```
* Updated declarations:

  ```yacc
  TYPE ID '=' expr ';' { $$ = createDecl($2, $4); free($2); }
  ```
* Expanded `for_loop` grammar to explicitly define `for_init`, `for_cond`, and `for_update`.

---

## Week 3 — TAC Enhancements & Optimization

### Version 1 — TAC Improvements

**Files Modified:** `tac.c`, `tac.h`, `parser.y`

#### `tac.h`

* Added TAC opcodes for arithmetic and relational operators:
  `-`, `*`, `/`, `==`, `!=`, `>`, `<`, `>=`, `<=`.

#### `tac.c`

* Fixed numeric literal formatting (`%g` for doubles).
* Handled `NODE_EXPR_LIST` for `print()` expressions.
* Mapped string operators to TAC operations.
* Added null checks to prevent dereferencing null operands.
* Extended `optimizeTAC()` for constant folding and propagation.
* Improved TAC printing for readability.

#### `parser.y`

* Allowed `TYPE ID` and `TYPE ID = expr` in loop initializers.
* Recognized `i++` and `i--` in loop updates.

**Validation:**
Successfully compiled `test.c` through all phases:
Lex → Parse → AST → TAC → Optimization → MIPS.
Confirmed constant folding and propagation.

---

### Version 2 — Code Generation Enhancements (`codegen.c`)

#### Additions

```c
#include <string.h>  // Added for strcmp
static int labelCount = 0;  // Unique label generation
```

#### Unary Operators

Support for logical NOT (`!`) and negation (`-`):

```c
case NODE_UNARY: {
    genExpr(node->data.unary.expr);
    int reg = tempReg - 1;
    if (strcmp(node->data.unary.op, "!") == 0)
        fprintf(output, "    sltiu $t%d, $t%d, 1\n", reg, reg);
    else if (strcmp(node->data.unary.op, "-") == 0)
        fprintf(output, "    neg $t%d, $t%d\n", reg, reg);
    break;
}
```

#### Short-Circuit Logical Ops

Efficient boolean evaluation for `&&` and `||`:

```c
if (strcmp(node->data.binop.op, "&&") == 0) { ... }
else if (strcmp(node->data.binop.op, "||") == 0) { ... }
```

#### Control Flow Statements

Added MIPS generation for:

* `while`
* `if` / `else`
* `for`
  Each uses label-based control flow.

#### Stack Frame Fix

```c
fprintf(output, "    addi $sp, $sp, -408\n");
fprintf(output, "    sw $ra, 404($sp)\n");
fprintf(output, "    sw $fp, 400($sp)\n");
```

Ensures proper stack layout and prevents memory corruption.

#### `test.c` Updates

Added `print()` calls for debugging and flow visualization.

**Stack Layout:**

```
404($sp) - Return Address ($ra)
400($sp) - Frame Pointer ($fp)
0–396($sp) - Local Variables (100 slots)
```

---

### Version 3 — Expression & Print Fixes

#### `codegen.c`

Support for `NODE_EXPR_LIST`:

```c
case NODE_EXPR_LIST:
    if (node->data.exprlist.first)
        genExpr(node->data.exprlist.first);
    break;
```

Fixed fragile print logic:

```c
genExpr(node->data.expr);
if (tempReg == 0) fprintf(output, "    move $a0, $zero\n");
else fprintf(output, "    move $a0, $t%d\n", tempReg - 1);
fprintf(output, "    li $v0, 1\n    syscall\n    li $v0, 4\n    la $a0, newline\n    syscall\n");
tempReg = 0;
```

---

## Week 4 — Array Declarations & Symbol Table

### `parser.y`

* Added array declaration, assignment, and access syntax:

  * `int [10]`
  * `int [10] = arr`
  * Assignment of arrays to variables
* Extended grammar:

  ```yacc
  expr: ... | TYPE '[' INT ']' { /* int[10] */ }
  assign: ... | ID '=' TYPE '[' INT ']' { /* arr = int[10]; */ }
  decl: TYPE '[' INT ']' ID { /* int[10] arr; */ }
  ```

### `ast.h` & `ast.c`

* Added **three new AST nodes**:

  * `NODE_ARRAY_DECL`
  * `NODE_ARRAY_ASSIGN`
  * `NODE_ARRAY_ACCESS`
* Added struct definitions and constructors for these nodes.
* Integrated array nodes into AST traversal and printing.

  ```c
  Node* createArrayDecl(const char* name, int size);
  Node* createArrayAssign(const char* name, Node* index, Node* value);
  Node* createArrayAccess(const char* name, Node* index);
  ```

### `symtab.h`

* Added symbol entries for arrays:

  * Tracks `isArray`, `size`, and memory `offset`.
* Provides APIs for scalar and array management.

### `symtab.c`

* Implemented **chained hash table** (djb2 hash).
* Handles insertions, lookups, and collisions efficiently.
* Stores array metadata and offset tracking.
* Added global visibility:

  ```c
  extern SymbolTable symtab;
  ```
* Ensures `main.c` and `codegen.c` can access symbol information.

### Other Updates

#### `improved_scanner.l`

* Removed unconditional token debug printing.
* Added `lex_debug` flag for optional token trace output.

#### `tac.c`

* Consolidated duplicate switch cases.
* Added TAC generation for array declarations and assignments:

  * `NODE_ARRAY_DECL` → `DECL arr`
  * `NODE_ARRAY_ASSIGN` → `TAC_ASSIGN`
* Improved TAC readability and fixed compile warnings.

#### `codegen.c`

* Implemented array load/store generation:

  ```c
  fprintf(output, "    la $t%d, %s($fp)\n", reg, node->data.array.name);
  ```
* Switched from fixed stack prologue to dynamic stack allocation based on:

  ```c
  symtab.nextOffset
  ```
* Automatically reserves correct memory for arrays.

#### `main.c`

* Added optional flags:

  * `--symtab` → print symbol table diagnostics
  * `--debug` → enable parser/lexer debugging
* Cleaned argument parsing and integrated diagnostics.

---

## Build Instructions

*Execute:
```bash
Make clean && make &&  ./minicompiler test.c output.s
```

*To Print result: 
```bash
spim output.s 
```

*To show assembly code: 
```bash
cat output.s
```	


---

## Summary of Major Features

* Rich token, operator, and keyword support.
* Parser supports expressions, loops, and array declarations.
* Full AST representation for scalars, expressions, and arrays.
* TAC generation and optimization with constant folding.
* MIPS code generation for arithmetic, control flow, and arrays.
* Hash-based symbol table with array metadata tracking.

---

## Next Steps

* Implement user-defined functions and return handling.
* Add TAC-level array indexing and propagation optimizations.
* Expand optimizer (dead-code elimination, loop unrolling).
* Improve compiler diagnostics and AST visualization.

