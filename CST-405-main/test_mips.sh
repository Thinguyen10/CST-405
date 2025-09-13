#!/bin/bash

# MIPS Testing Script for C-Minus Compiler
# Tests generated MIPS assembly with SPIM simulator

echo "=================================="
echo "MIPS Assembly Testing Script"
echo "=================================="
echo

if ! command -v spim &> /dev/null; then
    echo "SPIM not found. Installing..."
    brew install spim
    echo
fi

if [ $# -eq 0 ]; then
    echo "Usage: $0 <cminus_file.cm> [compiler_options]"
    echo
    echo "Examples:"
    echo "  $0 tests/simple.cm"
    echo "  $0 tests/factorial.cm -O2"
    echo "  $0 tests/fibonacci.cm -p"
    exit 1
fi

cminus_file="$1"
shift
compiler_options="$@"

# Check if C-Minus file exists
if [ ! -f "$cminus_file" ]; then
    echo "Error: C-Minus file '$cminus_file' not found"
    exit 1
fi

# Get base filename without extension
base_name="${cminus_file%.cm}"
mips_file="${base_name}.s"

echo "Step 1: Compiling C-Minus to MIPS..."
echo "Command: ./cminus $compiler_options $cminus_file"
echo

# Compile C-Minus to MIPS
./cminus $compiler_options "$cminus_file"

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

if [ ! -f "$mips_file" ]; then
    echo "Error: MIPS file '$mips_file' not generated"
    exit 1
fi

echo
echo "Step 2: Testing MIPS assembly with SPIM..."
echo "MIPS file: $mips_file"
echo "File size: $(wc -l < "$mips_file") lines"
echo

# Show the MIPS code structure
echo "--- MIPS Code Structure ---"
echo "Data section:"
grep -A 5 "^\.data" "$mips_file" | head -6
echo
echo "Functions found:"
grep "^[a-zA-Z_][a-zA-Z0-9_]*:" "$mips_file" | sed 's/:/ /' | awk '{print "  - " $1}'
echo

# Test with SPIM
echo "--- SPIM Simulation ---"
echo "Loading and running with SPIM..."
spim -file "$mips_file"

echo
echo "=== Testing Complete ==="
echo "Generated: $mips_file"
echo "SPIM test: Complete"