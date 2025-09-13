# Working Factorial Program in MIPS
# This is what the compiler should generate

.data
newline: .asciiz "\n"
prompt: .asciiz "Enter a number: "

.text
.globl main

# Factorial function: calculates n!
# Input: $a0 = n
# Output: $v0 = n!
factorial:
    # Base case: if n <= 1, return 1
    blez $a0, base_case
    beq $a0, 1, base_case
    
    # Recursive case: n * factorial(n-1)
    # Save registers
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $a0, 0($sp)
    
    # Calculate factorial(n-1)
    addi $a0, $a0, -1
    jal factorial
    
    # Restore n and multiply
    lw $a0, 0($sp)
    mul $v0, $v0, $a0
    
    # Restore return address
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra

base_case:
    li $v0, 1
    jr $ra

main:
    # Print prompt
    li $v0, 4
    la $a0, prompt
    syscall
    
    # Read number
    li $v0, 5
    syscall
    move $a0, $v0
    
    # Calculate factorial
    jal factorial
    
    # Print result
    move $a0, $v0
    li $v0, 1
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall