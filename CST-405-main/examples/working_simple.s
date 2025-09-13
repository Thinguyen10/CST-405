# Working Simple Arithmetic Program in MIPS
# This demonstrates what simple.cm should generate

.data
newline: .asciiz "\n"
prompt: .asciiz "Enter a number: "
msg_sum: .asciiz "Sum: "
msg_diff: .asciiz "Difference: "
msg_prod: .asciiz "Product: "
msg_quot: .asciiz "Quotient: "

.text
.globl main

main:
    # Get first number
    li $v0, 4
    la $a0, prompt
    syscall
    
    li $v0, 5
    syscall
    move $t0, $v0    # a = input()
    
    # Get second number
    li $v0, 4
    la $a0, prompt
    syscall
    
    li $v0, 5
    syscall
    move $t1, $v0    # b = input()
    
    # Calculate sum = a + b
    add $t2, $t0, $t1
    
    # Print "Sum: "
    li $v0, 4
    la $a0, msg_sum
    syscall
    
    # Print sum
    li $v0, 1
    move $a0, $t2
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Calculate difference = a - b
    sub $t3, $t0, $t1
    
    # Print "Difference: "
    li $v0, 4
    la $a0, msg_diff
    syscall
    
    # Print difference
    li $v0, 1
    move $a0, $t3
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Calculate product = a * b
    mul $t4, $t0, $t1
    
    # Print "Product: "
    li $v0, 4
    la $a0, msg_prod
    syscall
    
    # Print product
    li $v0, 1
    move $a0, $t4
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Calculate quotient = a / b (with zero check)
    beqz $t1, skip_division
    div $t0, $t1
    mflo $t5
    j print_quotient
    
skip_division:
    li $t5, 0
    
print_quotient:
    # Print "Quotient: "
    li $v0, 4
    la $a0, msg_quot
    syscall
    
    # Print quotient
    li $v0, 1
    move $a0, $t5
    syscall
    
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    
    # Exit
    li $v0, 10
    syscall