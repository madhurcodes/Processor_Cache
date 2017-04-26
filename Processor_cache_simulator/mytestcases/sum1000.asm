# Stores numbers 1 to 1000 in memory and computes their sum later
# also doubles it at the end.

	
	addi $8, $0, 1000
	addi $9, $0, 1
	lui $10, 0x1001
Lab1:
	sw $9, 0($10)
	add $10, $10, 4
	add $9, $9, 1
	sub $8, $8, 1
	beq  $8, 0 , here
	b Lab1
	here:
	addi $8, $0, 1000
	addi $11, $0, 0
	lui $10, 0x1001
Lab2:
	lw $9, 0($10)
	add $11, $11, $9
	add $10, $10, 4
	sub $8, $8, 1
	beq $8, 0, there
	b Lab2
	there:
	add $12, $11, $11
