#!/bin/bash

# MIPS Execution Script
# Runs MIPS assembly code using various simulators

echo "=================================="
echo "MIPS Execution Options"
echo "=================================="

if [ $# -eq 0 ]; then
    echo "Usage: $0 <mips_file.s> [options]"
    echo
    echo "Options:"
    echo "  -spim       Use SPIM simulator (default)"
    echo "  -qtspim     Use QtSPIM GUI simulator"
    echo "  -mars       Use MARS simulator"
    echo "  -debug      Run with debugging enabled"
    echo "  -step       Step through execution"
    echo
    echo "Examples:"
    echo "  $0 examples/hello_mips.s"
    echo "  $0 tests/factorial.s -debug"
    echo "  $0 examples/hello_mips.s -qtspim"
    exit 1
fi

MIPS_FILE="$1"
shift

# Default options
SIMULATOR="spim"
DEBUG_MODE=false
STEP_MODE=false

# Parse options
while [[ $# -gt 0 ]]; do
    case $1 in
        -spim)
            SIMULATOR="spim"
            shift
            ;;
        -qtspim)
            SIMULATOR="qtspim"
            shift
            ;;
        -mars)
            SIMULATOR="mars"
            shift
            ;;
        -debug)
            DEBUG_MODE=true
            shift
            ;;
        -step)
            STEP_MODE=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check if file exists
if [ ! -f "$MIPS_FILE" ]; then
    echo "Error: File '$MIPS_FILE' not found"
    exit 1
fi

echo "File: $MIPS_FILE"
echo "Simulator: $SIMULATOR"
echo "Debug mode: $DEBUG_MODE"
echo "Step mode: $STEP_MODE"
echo

# Show file info
echo "--- MIPS File Info ---"
echo "Lines: $(wc -l < "$MIPS_FILE")"
echo "Size: $(wc -c < "$MIPS_FILE") bytes"
echo

# Check for undefined symbols (common issue)
echo "--- Symbol Check ---"
if grep -q "^[[:space:]]*lw.*[a-zA-Z_][a-zA-Z0-9_]*[[:space:]]*$" "$MIPS_FILE"; then
    echo "⚠️  Warning: Possible undefined symbols detected"
fi

if grep -q "^\.globl main" "$MIPS_FILE"; then
    echo "✓ Main function found"
else
    echo "⚠️  Warning: No main function found"
fi
echo

# Execute based on simulator choice
case $SIMULATOR in
    spim)
        echo "--- Running with SPIM ---"
        if command -v spim >/dev/null 2>&1; then
            if [ "$DEBUG_MODE" = true ]; then
                echo "Starting SPIM in debug mode..."
                spim
                echo "In SPIM console, type:"
                echo "  load \"$MIPS_FILE\""
                echo "  run"
                echo "  print \$v0"
                echo "  exit"
            else
                echo "Output:"
                echo "======="
                spim -file "$MIPS_FILE"
                echo "======="
            fi
        else
            echo "SPIM not installed. Install with: brew install spim"
        fi
        ;;
        
    qtspim)
        echo "--- Running with QtSPIM ---"
        if command -v qtspim >/dev/null 2>&1; then
            echo "Launching QtSPIM GUI..."
            qtspim "$MIPS_FILE" &
            echo "QtSPIM launched in background"
        else
            echo "QtSPIM not installed. Install with: brew install qtspim"
        fi
        ;;
        
    mars)
        echo "--- Running with MARS ---"
        if [ -f "Mars.jar" ]; then
            echo "Running with MARS simulator..."
            java -jar Mars.jar "$MIPS_FILE"
        else
            echo "MARS not found. Download Mars.jar from:"
            echo "http://courses.missouristate.edu/KenVollmar/mars/"
        fi
        ;;
esac

echo
echo "Execution complete!"