# Simple "Hello World" MIPS program
# This demonstrates basic MIPS execution

.data
hello_msg: .asciiz "Hello from MIPS!\n"
number: .word 42

.text
.globl main

main:
    # Print hello message
    li $v0, 4          # print string syscall
    la $a0, hello_msg  # load address of string
    syscall
    
    # Print a number
    li $v0, 1          # print integer syscall
    lw $a0, number     # load the number
    syscall
    
    # Print newline
    li $v0, 4          # print string syscall
    la $a0, newline    # load newline
    syscall
    
    # Exit program
    li $v0, 10         # exit syscall
    syscall

.data
newline: .asciiz "\n"