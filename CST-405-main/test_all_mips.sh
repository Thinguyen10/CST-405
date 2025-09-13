#!/bin/bash

# Test All MIPS Programs
# Compares compiler-generated vs working examples

echo "======================================="
echo "MIPS Program Comparison Test"
echo "======================================="
echo

echo "1. Testing Compiler-Generated MIPS Code:"
echo "----------------------------------------"

for file in tests/*.s; do
    if [ -f "$file" ] && [ -s "$file" ]; then
        echo "Testing $(basename $file)..."
        timeout 5 ./run_mips.sh "$file" 2>/dev/null | grep -A 10 "Output:" || echo "  ❌ Failed to execute or no output"
        echo
    fi
done

echo "2. Testing Working Example MIPS Code:"
echo "------------------------------------"

for file in examples/*.s; do
    if [ -f "$file" ] && [ -s "$file" ]; then
        echo "Testing $(basename $file)..."
        case $(basename $file) in
            working_factorial.s)
                echo "5" | timeout 5 ./run_mips.sh "$file" | grep -A 10 "Output:"
                ;;
            working_simple.s)
                printf "10\n5\n" | timeout 5 ./run_mips.sh "$file" | grep -A 10 "Output:"
                ;;
            *)
                timeout 5 ./run_mips.sh "$file" | grep -A 10 "Output:"
                ;;
        esac
        echo
    fi
done

echo "======================================="
echo "Summary:"
echo "- Compiler generates MIPS files but they have execution issues"
echo "- Working examples demonstrate correct MIPS structure"
echo "- This provides excellent educational opportunities"
echo "======================================="