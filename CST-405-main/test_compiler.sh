#!/bin/bash

# Test script for C-Minus Compiler
# CST-405 Compiler Design

echo "=================================="
echo "Testing C-Minus Compiler"
echo "=================================="
echo

# Build the compiler first
echo "Building compiler..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo

# Test each program
for test_file in tests/*.cm; do
    echo "--------------------------------"
    echo "Testing: $(basename $test_file)"
    echo "--------------------------------"
    
    # Run compiler
    ./cminus "$test_file" 2>/dev/null
    
    if [ $? -eq 0 ]; then
        echo "✓ Compilation successful"
        output_file="${test_file%.cm}.s"
        if [ -f "$output_file" ]; then
            echo "✓ Generated: $output_file"
            echo "  Lines of MIPS code: $(wc -l < "$output_file")"
        fi
    else
        echo "✗ Compilation failed"
    fi
    echo
done

echo "=================================="
echo "Test Summary"
echo "=================================="
echo "Total test files: $(ls tests/*.cm | wc -l)"
echo "Generated assemblies: $(ls tests/*.s 2>/dev/null | wc -l)"