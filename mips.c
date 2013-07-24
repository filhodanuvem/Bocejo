#include <stdio.h>
#include <string.h>
#include "mips.h"
#include "error.h"
#include "visitor_fragment.h"

static FILE* file_mips = NULL;
static int current_temp = 0;
void mips_constructor(char* file_name,Procedure* frag,label* label_main){
    create_mips_file(file_name);
    Visitor_mips* visit = new Visitor_mips(label_main);
    visit->visit(frag);
}

void mips_destructor(){
    add_instruction("\tli $v0, 10      #10 é o código para sair do programa");
    add_instruction("\tsyscall");
    fclose(file_mips);
}

void create_mips_file(char* file_name){
    file_mips = fopen(file_name,"w+");
    if(file_mips == NULL) lexical_error(E_FILE_ASM_NOT_FOUND,0);
}

void add_instruction(char* instruction){
    fprintf(file_mips,"%s \n",instruction);
    fprintf(stdout,"%s \n",instruction);
}
class ir_temp;

//Funcção para concatenação
char* toString(int val){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= 10)
    buf[i] = "0123456789abcdef"[val % 10];
    return &buf[i+1];
}




