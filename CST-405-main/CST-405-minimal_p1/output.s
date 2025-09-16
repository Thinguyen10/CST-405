.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared a at offset 0
    # Declared b at offset 4
    # Declared c at offset 8
    # Declared i at offset 12
    # Declared d at offset 16
    # Declared e at offset 20

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
