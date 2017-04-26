# Initializes  a ten member array and sorts it
# in descending order in memory it's an in place sorting algorithm(bubble sort)
# and as such does not take additional memory

lui $9, 0x1001
addi $8, $0, 10		
sw $8, 0($9)		
addi $8, $0, 2		
sw $8, 4($9)		
addi $8, $0, 8		
sw $8, 8($9)		
addi $8, $0, 7		
sw $8, 12($9)		
addi $8, $0, 5		
sw $8, 16($9)		
addi $8, $0, 6	
sw $8, 20($9)		
addi $8, $0, 4		
sw $8, 24($9)		
addi $8, $0, 3		
sw $8, 28($9)		
addi $8, $0, 9		
sw $8, 32($9)		
addi $8, $0, 1		
sw $8, 36($9)		

main:
	lui $t0, 0x1001
    add $t0, $t0, 40    
outterLoop:             
    add $t1, $0, $0     
	lui $a0, 0x1001
innerLoop:                  
    lw  $t2, 0($a0)         
    lw  $t3, 4($a0)         
    slt $t5, $t2, $t3       
    beq $t5, $0, continue   
    add $t1, $0, 1          
    sw  $t2, 4($a0)         
    sw  $t3, 0($a0)         
continue:
    addi $a0, $a0, 4            
    beq $a0, $t0, nnn
    b innerLoop

    nnn:
    beq $t1, $0, kkk

	b outterLoop
	kkk:
	
