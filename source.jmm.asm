 
	.text 
	.globl funcoes_fib_i 
funcoes_fib_i: 
# -------------- Prólogo de funcoes_fib_i ----------------- 
	subu   $sp, $sp, 84 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 80 
	sw $a0, 0($fp) 
	sw $a1, -4($fp) 
 
	li $t0, 1 
	sw $t0, -8($fp) 
 
 
	lw $t0, -4($fp) 
 
	li $t1, 1 
	bgt $t0, $t1, Label_8 
	j Label_10 
Label_10: 
 
	j Label_9 
Label_9: 
 
	li $t0, 0 
	sw $t0, -8($fp) 
Label_8: 
 
 
	lw $t0, -8($fp) 
 
	li $t1, 0 
	bne $t0, $t1, Label_3 
	j Label_7 
Label_7: 
 
	j Label_4 
Label_3: 
 
	#operacao entre duas constantes 
# -------------- Pré-chamada de funcoes_fib_i em funcoes_fib_i----------------- 
 
	lw $t2, 0($fp) 
	move $a0, $t2 
 
	#operacao entre duas constantes 
 
	lw $t3, -4($fp) 
 
	li $t4, 1 
	move $t8, $t2  
	sub $t2, $t3, $t4 
	move $a1, $t2 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_fib_i #passando a bola para o método 
# -------------- Epílogo de funcoes_fib_i (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t1, $v0 
# -------------- Pré-chamada de funcoes_fib_i em funcoes_fib_i----------------- 
 
	lw $t3, 0($fp) 
	move $a0, $t3 
 
	#operacao entre duas constantes 
 
	lw $t4, -4($fp) 
 
	li $t5, 2 
	move $t8, $t3  
	sub $t3, $t4, $t5 
	move $a1, $t3 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_fib_i #passando a bola para o método 
# -------------- Epílogo de funcoes_fib_i (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t2, $v0 
	move $t8, $t1  
	add $t1, $t1, $t2  
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
Label_4: 
 
	li $t1, 1 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
 
	.text 
	.globl funcoes_fatorial_i 
funcoes_fatorial_i: 
# -------------- Prólogo de funcoes_fatorial_i ----------------- 
	subu   $sp, $sp, 84 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 80 
	sw $a0, 0($fp) 
	sw $a1, -4($fp) 
 
	li $t0, 1 
	sw $t0, -8($fp) 
 
 
	lw $t0, -4($fp) 
 
	li $t1, 1 
	bgt $t0, $t1, Label_19 
	j Label_21 
Label_21: 
 
	j Label_20 
Label_20: 
 
	li $t0, 0 
	sw $t0, -8($fp) 
Label_19: 
 
 
	lw $t0, -8($fp) 
 
	li $t1, 0 
	bne $t0, $t1, Label_14 
	j Label_18 
Label_18: 
 
	j Label_15 
Label_14: 
 
	#operacao entre duas constantes 
 
	lw $t2, -4($fp) 
# -------------- Pré-chamada de funcoes_fatorial_i em funcoes_fatorial_i----------------- 
 
	lw $t4, 0($fp) 
	move $a0, $t4 
 
	#operacao entre duas constantes 
 
	lw $t5, -4($fp) 
 
	li $t6, 1 
	move $t8, $t4  
	sub $t4, $t5, $t6 
	move $a1, $t4 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_fatorial_i #passando a bola para o método 
# -------------- Epílogo de funcoes_fatorial_i (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t3, $v0 
	move $t8, $t1  
	mul $t1, $t2, $t3 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
Label_15: 
 
	li $t1, 1 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
 
	.text 
	.globl funcoes_bubbleSort_i 
funcoes_bubbleSort_i: 
# -------------- Prólogo de funcoes_bubbleSort_i ----------------- 
	subu   $sp, $sp, 104 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 100 
	sw $a0, 0($fp) 
	sw $a1, -4($fp) 
 
	li $t0, 0 
	sw $t0, -8($fp) 
 
	li $t0, 0 
	sw $t0, -12($fp) 
	sw $zero, -16($fp) 
Label_24: 
 
	li $t0, 1 
	sw $t0, -20($fp) 
 
 
	lw $t0, -8($fp) 
 
 
	lw $t2, -4($fp) 
	lw $t1, 0($t2) 
	blt $t0, $t1, Label_28 
	j Label_30 
Label_30: 
 
	j Label_29 
Label_29: 
 
	li $t1, 0 
	sw $t1, -20($fp) 
Label_28: 
 
 
	lw $t1, -20($fp) 
 
	li $t2, 0 
	bne $t1, $t2, Label_25 
	j Label_27 
Label_27: 
 
	j Label_26 
Label_25: 
Label_31: 
 
	li $t1, 1 
	sw $t1, -24($fp) 
 
 
	lw $t1, -12($fp) 
	li $t2, 0 #elemento neutro carregado 
 
	#operacao entre duas constantes 
 
 
	lw $t4, -4($fp) 
	lw $t3, 0($t4) 
 
	li $t5, 1 
	move $t8, $t2  
	sub $t2, $t3, $t5 
	blt $t1, $t2, Label_35 
	j Label_37 
Label_37: 
 
	j Label_36 
Label_36: 
 
	li $t1, 0 
	sw $t1, -24($fp) 
Label_35: 
 
 
	lw $t1, -24($fp) 
 
	li $t2, 0 
	bne $t1, $t2, Label_32 
	j Label_34 
Label_34: 
 
	j Label_33 
Label_32: 
 
	li $t1, 1 
	sw $t1, -28($fp) 
 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t3, -12($fp) 
 
	li $t4, 1 
	move $t8, $t2  
	add $t2, $t3, $t4  
 
	li $t3, 4 
	mul $t2, $t2, $t3 
 
	lw $t3, -4($fp) 
	add $t2, $t3, $t2  
	lw $t2, 0($t2) 
 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t4, -12($fp) 
 
	li $t5, 1 
	move $t8, $t3  
	add $t3, $t4, $t5  
 
	li $t4, 1 
	add $t3, $t3, $t4  
 
	li $t4, 4 
	mul $t3, $t3, $t4 
 
	lw $t4, -4($fp) 
	add $t3, $t4, $t3  
	lw $t3, 0($t3) 
	bgt $t2, $t3, Label_43 
	j Label_45 
Label_45: 
 
	j Label_44 
Label_44: 
 
	li $t1, 0 
	sw $t1, -28($fp) 
Label_43: 
 
 
	lw $t1, -28($fp) 
 
	li $t2, 0 
	bne $t1, $t2, Label_38 
	j Label_42 
Label_42: 
 
	j Label_39 
Label_38: 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t3, -12($fp) 
 
	li $t4, 1 
	move $t8, $t2  
	add $t2, $t3, $t4  
 
	li $t3, 4 
	mul $t2, $t2, $t3 
 
	lw $t3, -4($fp) 
	add $t2, $t3, $t2  
	lw $t2, 0($t2) 
	sw $t2, -16($fp) 
 
 
 
	#operacao entre duas constantes 
 
	lw $t2, -12($fp) 
 
	li $t3, 1 
	move $t8, $t1  
	add $t1, $t2, $t3  
 
	li $t2, 4 
	mul $t1, $t1, $t2 
 
	lw $t2, -4($fp) 
	add $t1, $t2, $t1  
 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t4, -12($fp) 
 
	li $t5, 1 
	move $t8, $t3  
	add $t3, $t4, $t5  
 
	li $t4, 1 
	add $t3, $t3, $t4  
 
	li $t4, 4 
	mul $t3, $t3, $t4 
 
	lw $t4, -4($fp) 
	add $t3, $t4, $t3  
	lw $t3, 0($t3) 
	sw $t3, 0($t1) 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t1, -12($fp) 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 1 
	add $t0, $t0, $t1  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
	lw $t1, -16($fp) 
	sw $t1, 0($t0) 
Label_39: 
 
	#operacao entre duas constantes 
 
	lw $t1, -12($fp) 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
	sw $t0, -12($fp) 
 
	j Label_31 
Label_33: 
 
	#operacao entre duas constantes 
 
	lw $t1, -8($fp) 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
	sw $t0, -8($fp) 
 
	li $t0, 0 
	sw $t0, -12($fp) 
 
	j Label_24 
Label_26: 
 
	li $t1, 1 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
	.rdata 
str48: 
	.asciiz "idade:" #reserva espaco para o literal 
	.rdata 
str49: 
	.asciiz "\n" #reserva espaco para o literal 
	.rdata 
str50: 
	.asciiz "curso:" #reserva espaco para o literal 
	.rdata 
str51: 
	.asciiz "\n" #reserva espaco para o literal 
 
	.text 
	.globl pessoa_toString 
pessoa_toString: 
# -------------- Prólogo de pessoa_toString ----------------- 
	subu   $sp, $sp, 76 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 72 
	sw $a0, 0($fp) 
 
	li $v0, 0x04 
	la $a0, str48 
	syscall 
 
 
	lw $t1, 0($fp) 
	lw $t0, 0($t1) 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str49 
	syscall 
	li $v0, 0x04 
	la $a0, str50 
	syscall 
 
 
	lw $t1, 0($fp) 
	lw $t0, 4($t1) 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str51 
	syscall 
	la $t3, str51 
	move $t9, $t3 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
 
	.text 
	.globl pessoa_envelhecer_i 
pessoa_envelhecer_i: 
# -------------- Prólogo de pessoa_envelhecer_i ----------------- 
	subu   $sp, $sp, 80 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 76 
	sw $a0, 0($fp) 
	sw $a1, -4($fp) 
 
	#operacao entre duas constantes 
 
	li $t2, 0 
 
	lw $t3, 0($fp) 
	move $t8, $t1  
	add $t1, $t2, $t3  
 
	#operacao entre duas constantes 
 
 
	lw $t4, 0($fp) 
	lw $t3, 0($t4) 
 
	lw $t5, -4($fp) 
	move $t8, $t2  
	add $t2, $t3, $t5  
	sw $t2, 0($t1) 
 
 
	lw $t2, 0($fp) 
	lw $t1, 0($t2) 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
 
	.text 
	.globl pessoa_get_idade 
pessoa_get_idade: 
# -------------- Prólogo de pessoa_get_idade ----------------- 
	subu   $sp, $sp, 76 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 72 
	sw $a0, 0($fp) 
 
 
	lw $t3, 0($fp) 
	lw $t2, 0($t3) 
	move $t9, $t2 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
	.rdata 
str64: 
	.asciiz "\n" #reserva espaco para o literal 
 
	.text 
	.globl main_mostra_vetor_i 
main_mostra_vetor_i: 
# -------------- Prólogo de main_mostra_vetor_i ----------------- 
	subu   $sp, $sp, 88 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 84 
	sw $a0, 0($fp) 
	sw $a1, -4($fp) 
 
	li $t2, 0 
	sw $t2, -8($fp) 
Label_57: 
 
	li $t2, 1 
	sw $t2, -12($fp) 
 
 
	lw $t2, -8($fp) 
 
 
	lw $t4, -4($fp) 
	lw $t3, 0($t4) 
	blt $t2, $t3, Label_61 
	j Label_63 
Label_63: 
 
	j Label_62 
Label_62: 
 
	li $t3, 0 
	sw $t3, -12($fp) 
Label_61: 
 
 
	lw $t3, -12($fp) 
 
	li $t4, 0 
	bne $t3, $t4, Label_58 
	j Label_60 
Label_60: 
 
	j Label_59 
Label_58: 
 
 
 
 
 
	#operacao entre duas constantes 
 
	lw $t2, -8($fp) 
 
	li $t3, 1 
	move $t8, $t1  
	add $t1, $t2, $t3  
 
	li $t2, 4 
	mul $t1, $t1, $t2 
 
	lw $t2, -4($fp) 
	add $t1, $t2, $t1  
	lw $t1, 0($t1) 
	move $a0, $t1 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str64 
	syscall 
 
	#operacao entre duas constantes 
 
	lw $t2, -8($fp) 
 
	li $t3, 1 
	move $t8, $t1  
	add $t1, $t2, $t3  
	sw $t1, -8($fp) 
 
	j Label_57 
Label_59: 
 
	li $t1, 1 
	move $t9, $t1 
	move $v0, $t9 
	lw $ra, 0($sp) 
	jr $ra 
	.rdata 
str68: 
	.asciiz "Vetor atual:\n" #reserva espaco para o literal 
	.rdata 
str72: 
	.asciiz "BubbleSort:\n" #reserva espaco para o literal 
	.rdata 
str83: 
	.asciiz "Elemento" #reserva espaco para o literal 
	.rdata 
str84: 
	.asciiz " do fibonacci: " #reserva espaco para o literal 
	.rdata 
str86: 
	.asciiz "\n" #reserva espaco para o literal 
	.rdata 
str88: 
	.asciiz "Digite um numero para o fat:\n" #reserva espaco para o literal 
	.rdata 
str91: 
	.asciiz "\nFatorial de " #reserva espaco para o literal 
	.rdata 
str92: 
	.asciiz " e: " #reserva espaco para o literal 
	.rdata 
str94: 
	.asciiz "\n" #reserva espaco para o literal 
	.rdata 
str99: 
	.asciiz "\nDigite sua idade :\n " #reserva espaco para o literal 
	.rdata 
str102: 
	.asciiz "\n" #reserva espaco para o literal 
	.rdata 
str105: 
	.asciiz "\nSua idade atual e :" #reserva espaco para o literal 
	.rdata 
str108: 
	.asciiz "\n" #reserva espaco para o literal 
 
	.text 
	.globl main 
main: 
# -------------- Prólogo de main_main ----------------- 
	subu   $sp, $sp, 100 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 96 
	sw $a0, 0($fp) 
	#alocação dinâmica 
	li $v0, 9 
	li $a0, 24 
	syscall 
	sw $v0, -4($fp) 
 
	lw $t0, -4($fp) 
 
	li $t2, 5 
	sw $t2, 0($t0) 
 
	lw $t0, -4($fp) 
	sw $zero, 4($t0) 
 
	lw $t0, -4($fp) 
	sw $zero, 8($t0) 
 
	lw $t0, -4($fp) 
	sw $zero, 12($t0) 
 
	lw $t0, -4($fp) 
	sw $zero, 16($t0) 
 
	lw $t0, -4($fp) 
	sw $zero, 20($t0) 
 
 
 
	#operacao entre duas constantes 
 
	li $t1, 0 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
	#operacao entre duas constantes 
 
	li $t2, 2 
 
	li $t3, 2 
	move $t8, $t1  
	add $t1, $t2, $t3  
	sw $t1, 0($t0) 
 
 
 
	#operacao entre duas constantes 
 
	li $t1, 1 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
 
 
	#operacao entre duas constantes 
 
	li $t2, 2 
 
	li $t3, 2 
	move $t8, $t1  
	div $t1, $t2, $t3 
 
	li $t2, 1 
	add $t1, $t1, $t2  
 
	li $t2, 1 
	add $t1, $t2, $t1  
	sw $t1, 0($t0) 
 
 
 
	#operacao entre duas constantes 
 
	li $t1, 2 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
 
 
 
	#operacao entre duas constantes 
 
	li $t2, 9 
 
	li $t3, 3 
	move $t8, $t1  
	sub $t1, $t2, $t3 
 
	li $t2, 2 
	div $t1, $t1, $t2 
 
	li $t2, 2 
	sub $t1, $t1, $t2 
 
	li $t2, 2 
	mul $t1, $t2, $t1 
	sw $t1, 0($t0) 
 
 
 
	#operacao entre duas constantes 
 
	li $t1, 3 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
 
 
	#operacao entre duas constantes 
 
	li $t2, 2 
 
	li $t3, 3 
	move $t8, $t1  
	add $t1, $t2, $t3  
 
	#operacao entre duas constantes 
 
	li $t2, 1 
 
	li $t3, 1 
	move $t8, $t1  
	add $t1, $t2, $t3  
	mul $t1, $t1, $t8 
 
	li $t2, 2 
	div $t1, $t1, $t2 
	sw $t1, 0($t0) 
 
 
 
	#operacao entre duas constantes 
 
	li $t1, 4 
 
	li $t2, 1 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	li $t1, 4 
	mul $t0, $t0, $t1 
 
	lw $t1, -4($fp) 
	add $t0, $t1, $t0  
 
	li $t1, 1 
	sw $t1, 0($t0) 
 
	li $t0, 0 
	sw $t0, -8($fp) 
 
	li $v0, 0x04 
	la $a0, str68 
	syscall 
# -------------- Pré-chamada de main_mostra_vetor_i em main_main----------------- 
 
	lw $t0, 0($fp) 
	move $a0, $t0 
 
	lw $t0, -4($fp) 
	move $a1, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal main_mostra_vetor_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	#alocação dinâmica 
	li $v0, 9 
	li $a0, 0 
	syscall 
	sw $v0, -12($fp) 
 
	li $v0, 0x04 
	la $a0, str72 
	syscall 
# -------------- Pré-chamada de funcoes_bubbleSort_i em main_main----------------- 
 
	lw $t0, -12($fp) 
	move $a0, $t0 
 
	lw $t0, -4($fp) 
	move $a1, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_bubbleSort_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
# -------------- Pré-chamada de main_mostra_vetor_i em main_main----------------- 
 
	lw $t1, 0($fp) 
	move $a0, $t1 
 
	lw $t1, -4($fp) 
	move $a1, $t1 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal main_mostra_vetor_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t1, $v0 
 
	li $t2, 0 
	sw $t2, -8($fp) 
Label_76: 
 
	li $t2, 1 
	sw $t2, -16($fp) 
 
 
	lw $t2, -8($fp) 
 
	li $t3, 4 
	blt $t2, $t3, Label_80 
	j Label_82 
Label_82: 
 
	j Label_81 
Label_81: 
 
	li $t2, 0 
	sw $t2, -16($fp) 
Label_80: 
 
 
	lw $t2, -16($fp) 
 
	li $t3, 0 
	bne $t2, $t3, Label_77 
	j Label_79 
Label_79: 
 
	j Label_78 
Label_77: 
 
	li $v0, 0x04 
	la $a0, str83 
	syscall 
 
	lw $t0, -8($fp) 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str84 
	syscall 
# -------------- Pré-chamada de funcoes_fib_i em main_main----------------- 
 
	lw $t0, -12($fp) 
	move $a0, $t0 
 
	lw $t0, -8($fp) 
	move $a1, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_fib_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str86 
	syscall 
 
	#operacao entre duas constantes 
 
	lw $t2, -8($fp) 
 
	li $t3, 1 
	move $t8, $t1  
	add $t1, $t2, $t3  
	sw $t1, -8($fp) 
 
	j Label_76 
Label_78: 
 
	li $v0, 0x04 
	la $a0, str88 
	syscall 
	sw $zero, -20($fp) 
	li $v0, 5 
	syscall 
 
	#operacao entre duas constantes 
 
	li $t1, 20 
	move $t8, $t0  
	sub $t0, $fp, $t1 
	sw $v0, 0($t0) 
 
	li $v0, 0x04 
	la $a0, str91 
	syscall 
 
	lw $t0, -20($fp) 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str92 
	syscall 
# -------------- Pré-chamada de funcoes_fatorial_i em main_main----------------- 
 
	lw $t0, -12($fp) 
	move $a0, $t0 
 
	lw $t0, -20($fp) 
	move $a1, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal funcoes_fatorial_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str94 
	syscall 
	#alocação dinâmica 
	li $v0, 9 
	li $a0, 8 
	syscall 
	sw $v0, -24($fp) 
 
	lw $t0, -24($fp) 
 
	li $t2, 21 
	sw $t2, 0($t0) 
 
	lw $t0, -24($fp) 
 
	li $t2, 22 
	sw $t2, 4($t0) 
 
# -------------- Pré-chamada de pessoa_toString em main_main----------------- 
 
	lw $t0, -24($fp) 
	move $a0, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal pessoa_toString #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
 
	li $v0, 0x04 
	la $a0, str99 
	syscall 
	li $v0, 5 
	syscall 
 
	#operacao entre duas constantes 
 
	li $t2, 20 
	move $t8, $t1  
	sub $t1, $fp, $t2 
	sw $v0, 0($t1) 
 
	#operacao entre duas constantes 
 
	li $t1, 0 
 
	lw $t2, -24($fp) 
	move $t8, $t0  
	add $t0, $t1, $t2  
 
	lw $t1, -20($fp) 
	sw $t1, 0($t0) 
 
	li $v0, 0x04 
	la $a0, str102 
	syscall 
# -------------- Pré-chamada de pessoa_toString em main_main----------------- 
 
	lw $t0, -24($fp) 
	move $a0, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal pessoa_toString #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
 
	li $v0, 0x04 
	la $a0, str105 
	syscall 
# -------------- Pré-chamada de pessoa_envelhecer_i em main_main----------------- 
 
	lw $t0, -24($fp) 
	move $a0, $t0 
 
	#operacao entre duas constantes 
# -------------- Pré-chamada de pessoa_get_idade em main_main----------------- 
 
	lw $t1, -24($fp) 
	move $a0, $t1 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal pessoa_get_idade #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t1, $v0 
 
	li $t3, 3 
	move $t8, $t0  
	add $t0, $t1, $t3  
	move $a1, $t0 
	sw $t0, 8($sp) 
	sw $t1, 12($sp) 
	sw $t2, 16($sp) 
	sw $t3, 20($sp) 
	sw $t4, 24($sp) 
	sw $t5, 28($sp) 
	sw $t6, 32($sp) 
	sw $t7, 36($sp) 
	sw $t8, 40($sp) 
	sw $t9, 44($sp) 
	jal pessoa_envelhecer_i #passando a bola para o método 
# -------------- Epílogo de main_main (saí da função)----------------- 
	move $t9, $fp 
	addi $t9, $t9, 4 
	lw $fp, 4($sp) 
	move $sp, $t9 
	lw $t0, 8($sp) 
	lw $t1, 12($sp) 
	lw $t2, 16($sp) 
	lw $t3, 20($sp) 
	lw $t4, 24($sp) 
	lw $t5, 28($sp) 
	lw $t6, 32($sp) 
	lw $t7, 36($sp) 
	lw $t8, 40($sp) 
	lw $t9, 44($sp) 
	move $t0, $v0 
	move $a0, $t0 
	li $v0, 0x01 
	syscall 
	li $v0, 0x04 
	la $a0, str108 
	syscall 
	li $v0, 10      #10 é o código para sair do programa 
	syscall 
