.data
newline: .asciiz "\n"

.text
.align 2
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -408  # 400 for vars + 8 for ra/fp
    # Setup stack frame
    sw $ra, 404($sp)   # Save return address at the top
    sw $fp, 400($sp)   # Save frame pointer below ra
    move $fp, $sp      # Set up frame pointer

    # Declared a at offset 0
    li $t0, 2
    sw $t0, 0($sp)
    # Declared b at offset 4
    li $t0, 3
    sw $t0, 4($sp)
    # Declared c at offset 8
    li $t0, 0
    sw $t0, 8($sp)
    # Declared i at offset 12
    li $t0, 0
    sw $t0, 12($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, 4($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, 8($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
loop_0:
    lw $t0, 12($sp)
    li $t1, 5
    slt $t0, $t1, $t0
    xori $t0, $t0, 1
    beq $t0, $zero, end_1
    lw $t1, 8($sp)
    lw $t2, 12($sp)
    add $t1, $t1, $t2
    sw $t1, 8($sp)
    lw $t0, 8($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, 12($sp)
    li $t1, 1
    add $t0, $t0, $t1
    sw $t0, 12($sp)
    j loop_0
end_1:
    # Declared j at offset 16
    li $t0, 0
    sw $t0, 16($sp)
loop_2:
    lw $t0, 16($sp)
    li $t1, 3
    slt $t0, $t0, $t1
    beq $t0, $zero, end_4
    lw $t1, 8($sp)
    lw $t2, 16($sp)
    add $t1, $t1, $t2
    sw $t1, 8($sp)
    lw $t0, 8($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
update_3:
    lw $t0, 16($sp)
    li $t1, 1
    add $t0, $t0, $t1
    sw $t0, 16($sp)
    j loop_2
end_4:
    # Declared e at offset 20
    lw $t0, 8($sp)
    lw $t1, 0($sp)
    sub $t0, $t0, $t1
    sw $t0, 20($sp)
    lw $t0, 20($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 4
    la $a0, newline
    syscall
    lw $t0, 20($sp)
    li $t1, 0
    slt $t0, $t1, $t0
    beq $t0, $zero, else_5
    lw $t1, 20($sp)
    li $t2, 10
    add $t1, $t1, $t2
    sw $t1, 20($sp)
    j end_6
else_5:
    lw $t0, 20($sp)
    li $t1, 10
    sub $t0, $t0, $t1
    sw $t0, 20($sp)
end_6:
    lw $t0, 20($sp)
    move $v0, $t0

    # Exit program
    move $sp, $fp      # Restore stack pointer
    lw $ra, 404($sp)   # Restore return address
    lw $fp, 400($sp)   # Restore frame pointer
    addi $sp, $sp, 408 # Deallocate stack space
    li $v0, 10
    syscall
