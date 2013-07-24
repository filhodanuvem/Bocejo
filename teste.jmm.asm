	.rdata 
str3: 
	.asciiz "ola mundo" #reserva espaco para o literal 
 
	.text 
	.globl main 
main: 
# -------------- Prólogo de foo_fazer ----------------- 
	subu   $sp, $sp, 76 
	sw     $ra, 0($sp) 
	sw     $fp, 4($sp) 
	addiu   $fp, $sp, 72 
	sw $a0, 0($fp) 
 
	li $v0, 0x04 
	la $a0, str3 
	syscall 
	li $v0, 10      #10 é o código para sair do programa 
	syscall 
