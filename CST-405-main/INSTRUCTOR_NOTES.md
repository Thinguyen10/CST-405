# Instructor Notes - C-Minus Compiler

## Overview
This is a complete educational compiler for the C-Minus language, designed for CST-405 Compiler Design course. It demonstrates all major phases of compilation using traditional tools (Flex/Bison) combined with hand-written C code.

## Key Educational Features

### 1. Complete Compilation Pipeline
- **Lexical Analysis** - Flex-based tokenizer with clear token definitions
- **Syntax Analysis** - Bison parser with explicit grammar rules
- **AST Construction** - Shows difference between parse tree and AST
- **Symbol Table** - Hierarchical scope management
- **Semantic Analysis** - Type checking and validation
- **3-Address Code** - Clean intermediate representation
- **Optimization** - Multiple optimization passes
- **MIPS Generation** - Real assembly output

### 2. Teaching Points Demonstrated

#### Lexical Analysis (lexer.l)
- Regular expression patterns
- Token classification
- Comment handling
- Line/column tracking
- Error reporting

#### Parsing (parser.y)
- Context-free grammar
- Precedence and associativity
- AST construction during parsing
- Error recovery

#### AST vs Parse Tree (ast.c)
- Visual demonstration of the difference
- Tree traversal algorithms
- Memory management
- DOT output for GraphViz

#### Symbol Table (symtab.c)
- Hash table implementation
- Scope management
- Type information storage
- Memory offset calculation

#### Semantic Analysis (semantic.c)
- Type checking algorithms
- Function signature validation
- Variable usage tracking
- Error reporting with location

#### Intermediate Code (codegen.c)
- Three-address code format
- Temporary variable generation
- Label management
- Control flow translation

#### Optimization (optimize.c)
- Constant folding
- Dead code elimination
- Copy propagation
- Common subexpression elimination
- Peephole optimization

#### Code Generation (mips.c)
- Register allocation (simple)
- Stack frame management
- Function calling conventions
- System calls for I/O

## Building and Running

```bash
# Build the compiler
make

# Compile a C-Minus program
./cminus source.cm

# Show AST and traces
./cminus -p source.cm       # Show AST
./cminus -spac source.cm    # All traces
./cminus -O2 source.cm      # Optimize

# Run test suite
./test_compiler.sh
```

## Sample Programs Included

1. **simple.cm** - Basic arithmetic and control flow
2. **factorial.cm** - Recursion demonstration
3. **fibonacci.cm** - Arrays and loops
4. **gcd.cm** - Euclidean algorithm
5. **sort.cm** - Nested loops and arrays

## Classroom Use Suggestions

### Week 1-2: Lexical Analysis
- Study `lexer.l` specification
- Modify to add new tokens
- Implement string literals (exercise)

### Week 3-4: Parsing
- Study grammar in `parser.y`
- Trace parse tree construction
- Add new grammar rules (exercise)

### Week 5-6: AST and Symbol Tables
- Compare parse tree vs AST
- Implement AST visualization
- Extend symbol table for new types

### Week 7-8: Semantic Analysis
- Study type checking rules
- Add new semantic checks
- Implement constant expression evaluation

### Week 9-10: Intermediate Code
- Trace 3-address code generation
- Optimize code by hand
- Add new TAC instructions

### Week 11-12: Optimization
- Study optimization techniques
- Measure optimization effectiveness
- Implement new optimizations

### Week 13-14: Code Generation
- Study MIPS assembly output
- Trace register allocation
- Generate code for new constructs

## Common Student Exercises

1. **Add Boolean Type**
   - Extend lexer for true/false
   - Add to parser grammar
   - Update type system
   - Generate appropriate code

2. **Add For Loops**
   - Design syntax
   - Update grammar
   - Transform to while loop in AST
   - Test thoroughly

3. **Add Switch Statements**
   - Design syntax
   - Implement in parser
   - Generate jump table
   - Optimize consecutive cases

4. **Improve Register Allocation**
   - Study current simple allocation
   - Implement register spilling
   - Add live variable analysis
   - Use graph coloring (advanced)

5. **Add Strings**
   - Lexer support
   - Data segment allocation
   - String operations
   - Printf-style output

## Known Limitations (Educational Opportunities)

1. **Simple Register Allocation** - Good for teaching, room for improvement
2. **No Floating Point** - Could be added as major project
3. **Basic Error Recovery** - Opportunity to improve
4. **Limited Optimizations** - Many more could be added
5. **Single File Compilation** - Linking could be added

## Assessment Ideas

1. **Trace Execution** - Given code, show all compilation phases
2. **Add Features** - Implement new language constructs
3. **Fix Bugs** - Intentionally introduce bugs for debugging
4. **Optimize Code** - Hand-optimize 3-address code
5. **Write Programs** - Create C-Minus programs for specific tasks

## Debugging Tips

- Use `-p` flag to see AST structure
- Use `-c` flag to see 3-address code
- Check symbol table output with `-a`
- Compare optimized vs unoptimized with `-O0` vs `-O2`
- Use SPIM simulator to test MIPS output

## Resources

- Dragon Book (Aho, Sethi, Ullman)
- Flex & Bison by John Levine
- MIPS Assembly Language Programming
- SPIM Simulator Documentation

## Contact

This compiler was developed as an educational tool for CST-405. 
Feel free to modify and extend for your teaching needs.