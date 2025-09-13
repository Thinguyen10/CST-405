# Simple Math Operations in MIPS
# Demonstrates basic arithmetic

.data
msg1: .asciiz "Computing 5 + 3 = "
msg2: .asciiz "Computing 10 * 4 = "
newline: .asciiz "\n"

.text
.globl main

main:
    # Print first message
    li $v0, 4
    la $a0, msg1
    syscall
    
    # Calculate 5 + 3
    li $t0, 5
    li $t1, 3
    add $t2, $t0, $t1
    
    # Print result
    li $v0, 1
    move $a0, $t2
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Print second message
    li $v0, 4
    la $a0, msg2
    syscall
    
    # Calculate 10 * 4
    li $t0, 10
    li $t1, 4
    mul $t2, $t0, $t1
    
    # Print result
    li $v0, 1
    move $a0, $t2
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall