# MIPS Code Execution Guide

## Overview
This guide shows you how to execute MIPS assembly code on your Mac using various simulators.

## Available Simulators

### 1. SPIM (Command Line) - ✅ Already Installed
SPIM is the classic MIPS simulator, perfect for automated testing.

#### Quick Execution:
```bash
# Run MIPS file directly
spim -file examples/hello_mips.s

# Using our script
./run_mips.sh examples/hello_mips.s
```

#### Interactive Mode:
```bash
spim
(spim) load "examples/hello_mips.s"
(spim) run
(spim) print $v0    # Check return value
(spim) print $a0    # Check argument register
(spim) exit
```

### 2. QtSPIM (GUI Version)
Install the graphical version for easier debugging:

```bash
brew install qtspim
qtspim examples/hello_mips.s
```

Or use our script:
```bash
./run_mips.sh examples/hello_mips.s -qtspim
```

### 3. MARS (Java-based)
Download and run MARS for advanced features:

```bash
# Download from: http://courses.missouristate.edu/KenVollmar/mars/
java -jar Mars.jar examples/hello_mips.s
```

## Working Examples

### 1. Hello World
```bash
./run_mips.sh examples/hello_mips.s
```
Output: `Hello from MIPS! 42`

### 2. Simple Math
```bash
./run_mips.sh examples/simple_math.s
```
Output: `Computing 5 + 3 = 8`, `Computing 10 * 4 = 40`

## Debugging Your MIPS Code

### Common Issues:

1. **Undefined Symbols**
   ```bash
   # Error: "The following symbols are undefined: n"
   # Solution: Make sure all variables are declared in .data section
   ```

2. **Missing Main Function**
   ```bash
   # Error: No entry point
   # Solution: Add .globl main and main: label
   ```

3. **Invalid Instructions**
   ```bash
   # Error: Invalid instruction
   # Solution: Check MIPS instruction syntax
   ```

### Debugging Commands:

```bash
# Debug mode - step through execution
./run_mips.sh examples/hello_mips.s -debug

# Check file before execution
./run_mips.sh examples/hello_mips.s
```

## MIPS Syscalls Reference

Common syscalls for I/O:

```mips
# Print integer
li $v0, 1
li $a0, 42        # number to print
syscall

# Print string
li $v0, 4
la $a0, message   # address of string
syscall

# Read integer
li $v0, 5
syscall           # result in $v0

# Exit program
li $v0, 10
syscall
```

## Creating Working MIPS Code

### Template:
```mips
.data
# Declare variables here
message: .asciiz "Hello World!"
number: .word 42

.text
.globl main

main:
    # Your code here
    
    # Always end with exit
    li $v0, 10
    syscall
```

### Variables:
```mips
.data
integer_var: .word 42
array_var: .word 1, 2, 3, 4, 5
string_var: .asciiz "Hello"
```

### Functions:
```mips
my_function:
    # Function prologue
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    
    # Function body here
    
    # Function epilogue
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
```

## Testing Your C-Minus Generated Code

The compiler generates MIPS code, but it may need fixes:

### 1. Compile C-Minus to MIPS:
```bash
./cminus tests/simple.cm
```

### 2. Check the generated code:
```bash
cat tests/simple.s
```

### 3. Try to run it:
```bash
./run_mips.sh tests/simple.s
```

### 4. If it fails, compare with working examples:
```bash
# Compare structure
diff examples/hello_mips.s tests/simple.s
```

## Educational Uses

### For Students:
1. **Learn MIPS Assembly** - Start with working examples
2. **Debug Programs** - Use interactive SPIM mode
3. **Understand Architecture** - See how high-level code becomes assembly
4. **Compare Compilers** - See how different algorithms generate code

### For Instructors:
1. **Demonstrate Compilation** - Show source-to-assembly translation
2. **Teach Architecture** - MIPS is simple and educational
3. **Debug Assignment** - Help students understand their compiler output
4. **Performance Analysis** - Count instructions, analyze efficiency

## Quick Reference

### Execute MIPS Code:
```bash
# Basic execution
./run_mips.sh file.s

# With GUI debugger
./run_mips.sh file.s -qtspim

# Interactive debugging
./run_mips.sh file.s -debug
```

### Check MIPS Code:
```bash
# Check syntax and structure
./run_mips.sh file.s

# View code
cat file.s | head -20
```

### Common MIPS Instructions:
- `li $t0, 5` - Load immediate
- `add $t0, $t1, $t2` - Add registers  
- `lw $t0, 0($t1)` - Load word from memory
- `sw $t0, 0($t1)` - Store word to memory
- `beq $t0, $t1, label` - Branch if equal
- `j label` - Jump to label
- `jal function` - Jump and link (function call)
- `jr $ra` - Jump register (return)

This setup gives you complete MIPS execution capabilities for educational and testing purposes!