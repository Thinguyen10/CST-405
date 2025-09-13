# C-Minus Compiler Makefile
# CST-405 Compiler Design Course

CC = gcc
CFLAGS = -Wall -g -I./include
LEX = flex
YACC = bison
YFLAGS = -d -v

# Source files
LEXER = src/lexer.l
PARSER = src/parser.y
SOURCES = src/main.c src/ast.c src/symtab.c src/semantic.c \
          src/codegen.c src/optimize.c src/mips.c src/util.c

# Generated files
LEX_C = src/lex.yy.c
PARSER_C = src/parser.tab.c
PARSER_H = src/parser.tab.h

# Object files
OBJECTS = $(SOURCES:.c=.o) $(LEX_C:.c=.o) $(PARSER_C:.c=.o)

# Executable
TARGET = cminus

# Default target
all: $(TARGET)

# Build the compiler
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Generate lexer
$(LEX_C): $(LEXER)
	$(LEX) -o $@ $<

# Generate parser
$(PARSER_C) $(PARSER_H): $(PARSER)
	$(YACC) $(YFLAGS) -o $(PARSER_C) $<

# Compile C files
%.o: %.c $(PARSER_H)
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependencies
src/main.o: include/globals.h include/ast.h include/symtab.h
src/ast.o: include/ast.h include/globals.h
src/symtab.o: include/symtab.h include/globals.h
src/semantic.o: include/semantic.h include/ast.h include/symtab.h
src/codegen.o: include/codegen.h include/ast.h include/symtab.h
src/optimize.o: include/optimize.h include/codegen.h
src/mips.o: include/mips.h include/codegen.h
src/util.o: include/util.h include/globals.h

# Test targets
test: $(TARGET)
	@echo "Running test suite..."
	@for test in tests/*.cm; do \
		echo "Testing $$test..."; \
		./$(TARGET) $$test; \
	done

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS) $(LEX_C) $(PARSER_C) $(PARSER_H)
	rm -f src/*.o parser.output

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean test install