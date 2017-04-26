# computes fibonacci series (10 terms (can change) ) and stores in memory, at end multiplies last two generated numbers and stores to t1
lui $8, 0x1001
addi $4, $0, 1	
sw $4, 0($8)
addi $8, $8, 4	
sw $4, 0($8)
addi $12, $0, 1	
addi $13, $0, 10

jal calc
addi $12, $12, 1
beq $12, $13, exit

calc:
lw  $10, -4($8)
lw $11, 0($8)
add $11, $11, $10
add $10, $8, 4
move $8, $10
sw $11, 0($8)
jr $ra
exit:
lw  $10, -4($8)
lw $11, 0($8)
mult $10, $11
mflo $9
