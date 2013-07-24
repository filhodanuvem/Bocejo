#ifndef MIPS_H_INCLUDED
#define MIPS_H_INCLUDED

#define MAX_CONST_SIZE 400

#define NUM_REG_PROLOGO 18 // 10 $t , $fp , $ra, 4 $a
#define PRINT_FLUX_MIPS 0


class Procedure;
class label;

void mips_constructor(char* file_name,Procedure* frag,label* label_main);
void create_mips_file(char* file_name);
void add_instruction(char* instruction);
void mips_destructor();
char* toString(int val);

#endif // MIPS_H_INCLUDED
