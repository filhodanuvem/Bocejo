/**
* visitor_ir.cpp
* @author Claudson Oliveira
* @date   21 de junho de 2011
*
* Arquivo que implementa todo o módulo relacionado a RI
*
**/
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <string.h>
#include <string>
#include "visitor_ir.h"
#include "lexical.h"
#include "semantic.h"
#include "error.h"
#include "symbols_tables.h"
#include "abstract_syntax_tree.h"
#include "visitor_fragment.h"
#include "visitor_translateAsaIR.h"
#include "IR.h"
#include "mips.h"
/**
* Visitor que trata de cuspir o código relacionado em mips
*/
static char* registers[] = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7"};

Visitor_muncher::Visitor_muncher(){
    current_temp = 0;
    dest_temp = 0 ;
    this->bocejo_helper = 0;
}

// munchers que retornam o mips
char* Visitor_muncher::munch(ir_binop* node){
    if(PRINT_FLUX_MIPS) add_instruction("#BINOP-------------------");
    else add_instruction("");
    char instr[MAX_CONST_SIZE];
    char* dest = NULL;
    char* t0   = NULL;
    char* t1   = NULL;

    int first = 0 ; // marca se esta passando por este metodo pelo primeira vez
    if(!this->bocejo_helper){
        first = 1 ;
        this->bocejo_helper = 1;
    }

    ir_binop* binop1 = dynamic_cast<ir_binop*>(node->attr_exp1);
    ir_binop* binop2 = dynamic_cast<ir_binop*>(node->attr_exp2);

    if(binop1 && binop2){
        // expressão dupla, como uma soma de somas
        t0 = this->munch(node->attr_exp1); // 1 e 2
        t1 = this->munch(node->attr_exp2); // 3 e 4
        dest = registers[current_temp];
    }else{
        if(binop1 || binop2){
            // um deles é outra expressão aritmética
            if(binop1){
                t0   = this->munch(node->attr_exp1);
                dest = t0 ;
                current_temp++;
                t1 = this->munch(node->attr_exp2);
                current_temp--;
            }

            if(binop2){
                t1   = this->munch(node->attr_exp2);
                dest = t1 ;
                current_temp++;
                t0 = this->munch(node->attr_exp1);
                current_temp--;
            }
            current_temp--;
        }else{
            // os dois são constantes
            if(current_temp < 0 ) current_temp = 0;
            dest = registers[current_temp++];
            add_instruction("\t#operacao entre duas constantes");
            t0   = this->munch(node->attr_exp1);
            if(dynamic_cast<ir_call*>(node->attr_exp1) || dynamic_cast<ir_call*>(node->attr_exp2)) current_temp++;
            t1   = this->munch(node->attr_exp2);
            current_temp-=3 ; // usou tres registradores, o de destino ainda esta alocado
            if((node->attr_op == T_MULT || node->attr_op == T_DIV)&&first){
                sprintf(instr,"\tli $t8, 1",dest);
            }else sprintf(instr,"\tmove $t8, %s ",dest);
            add_instruction(instr);
        }

        switch(node->attr_op){
            case T_PLUS : sprintf(instr,"\tadd %s, %s, %s ",dest,t0,t1); break;
            case T_MINUS: sprintf(instr,"\tsub %s, %s, %s",dest,t0,t1); break;
            case T_MULT : sprintf(instr,"\tmul %s, %s, %s",dest,t0,t1);break;
            case T_DIV  : sprintf(instr,"\tdiv %s, %s, %s",dest,t0,t1);break;
            case T_OR_UNI: sprintf(instr,"\tor %s, %s, %s",dest,t0,t1);break;
            case T_AND_UNI: sprintf(instr,"\tand %s, %s, %s",dest,t0,t1);break;
            case T_OR: sprintf(instr,"\txor %s, %s, %s",dest,t0,t1);break;
            case T_NOR_UNI: sprintf(instr,"\tnor %s, %s, %s",dest,t0,t1);break;
            case T_MOD  :
                sprintf(instr,"\tdiv %s, %s, %s",dest,t0,t1);
                add_instruction(instr);
                sprintf(instr,"\tmfhi %s",dest);
            break;
        }
        add_instruction(instr);
    }
    if((binop1 && binop2 )){
        switch(node->attr_op){
            case T_PLUS : sprintf(instr,"\tadd %s, %s, $t8 ",dest,dest); break;
            case T_MINUS: sprintf(instr,"\tsub %s, %s, $t8",dest,dest); break;
            case T_MULT : sprintf(instr,"\tmul %s, %s, $t8",dest,dest);break;
            case T_DIV  : sprintf(instr,"\tdiv %s, %s, $t8",dest,dest);break;
            case T_OR_UNI: sprintf(instr,"\tor %s, %s, $t8",dest,dest);break;
            case T_AND_UNI: sprintf(instr,"\tand %s, %s, $t8",dest,dest);break;
            case T_OR: sprintf(instr,"\txor %s, %s, $t8",dest,dest);break;
            case T_NOR_UNI: sprintf(instr,"\tnor %s, %s, $t8",dest,dest);break;
            case T_MOD  :
                sprintf(instr,"\tdiv %s, %s, $t8",dest,dest);
                add_instruction(instr);
                sprintf(instr,"\tmfhi %s",dest);
            break;
        }
        add_instruction(instr);
    }
    return dest;
}
char* Visitor_muncher::munch(ir_call* node){
    char instr[MAX_CONST_SIZE];
    if(strcmp(node->attr_name->attr_name->attr_string,"MALLOC") == 0){
        ir_const* bytes_length = dynamic_cast<ir_const*>(node->attr_el->attr_expr);
        if(bytes_length){
            add_instruction("\t#alocação dinâmica");
            add_instruction("\tli $v0, 9");
            sprintf(instr,"\tli $a0, %d",bytes_length->attr_i);
            add_instruction(instr);
            add_instruction("\tsyscall");
        }
    }else{
        //------------ PRÉ-CHAMADA ------------------
         sprintf(instr,"# -------------- Pré-chamada de %s em %s-----------------",node->attr_name->attr_name->attr_string,this->current_frame->attr_label->attr_string);
         add_instruction(instr);
         char* t0 = NULL;
         ir_expr_list* list = node->attr_el;
         int reg_number = 0 ;
         while(list != NULL){
            t0   = this->munch(list->attr_expr);
            current_temp--;
            sprintf(instr,"\tmove $a%d, %s",reg_number,t0);
            add_instruction(instr);
            list = list->attr_expr_list;
            reg_number++;
            if(reg_number == 4) break;
        }
        int offset = 8;
        reg_number = 0 ;
        while(reg_number < 10){
            sprintf(instr,"\tsw $t%d, %d($sp)",reg_number,offset);
            add_instruction(instr);
            offset += 4;
            reg_number++;
        }
        sprintf(instr,"\tjal %s #passando a bola para o método",node->attr_name->attr_name->attr_string);
        add_instruction(instr);
        // ---------------  EPÍLOGO ---------------------------
        sprintf(instr,"# -------------- Epílogo de %s (saí da função)-----------------",this->current_frame->attr_label->attr_string);
        add_instruction(instr);
        reg_number =0;
        offset = 8;
        add_instruction("\tmove $t9, $fp");
        add_instruction("\taddi $t9, $t9, 4");
        add_instruction("\tlw $fp, 4($sp)");
        add_instruction("\tmove $sp, $t9");
        while(reg_number < 10){
            sprintf(instr,"\tlw $t%d, %d($sp)",reg_number,offset);
            add_instruction(instr);
            offset += 4;
            reg_number++;
        }
        if(current_temp < 0) current_temp =0;
        t0 = registers[current_temp++];
        sprintf(instr,"\tmove %s, $v0",t0);
        add_instruction(instr);
        return t0;
    }
    return "$v0"; // retorna o registrador que guarda a posicação na heap
}
char* Visitor_muncher::munch(ir_cjump* node){
    char instr[MAX_CONST_SIZE];
    //if(!this->bocejo_helper) this->bocejo_helper = 1;
    if(PRINT_FLUX_MIPS) add_instruction("#CJUMP-------------------");
    else add_instruction("");
    // No caso do CJump estar envolvido a uma expressão aritmética
    // é preciso tratar algumas peculiaridades na seleção de regitradores
    // como por exemplo, se a comparação for do tipo
    // binop == const é preciso ficar atento pois quando o munch do binop
    // voltar, é preciso incrementar o proximo registrador a ser pego
    ir_binop* binop1 = dynamic_cast<ir_binop*>(node->attr_expr1);
    ir_binop* binop2 = dynamic_cast<ir_binop*>(node->attr_expr2);
    ir_temp* temp1 = dynamic_cast<ir_temp*>(node->attr_expr1);
    ir_temp* temp2 = dynamic_cast<ir_temp*>(node->attr_expr2);
    if(binop1){
        // elemento neutro carregado
        int neutro = (binop1->attr_op == T_MULT || binop1->attr_op == T_DIV)?1:0;
        sprintf(instr,"\tli %s, %d #elemento neutro carregado",registers[++current_temp],neutro);
        add_instruction(instr);
    }

    char* t0 = this->munch(node->attr_expr1);
    if(temp1)t0 = "$t9";
    if(binop1) current_temp++;
    if(binop2){
        // elemento neutro carregado
        int neutro = (binop2->attr_op == T_MULT || binop2->attr_op == T_DIV)?1:0;
        sprintf(instr,"\tli %s, %d #elemento neutro carregado",registers[current_temp],neutro);
        add_instruction(instr);
    }
    char* t1 = this->munch(node->attr_expr2);
    switch(node->attr_op){
        case T_EQUAL:     sprintf(instr,"\tbeq %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
        case T_NOT_EQUAL: sprintf(instr,"\tbne %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
        case T_GREATER_E: sprintf(instr,"\tbge %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
        case T_GREATER_T: sprintf(instr,"\tbgt %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
        case T_LESS_E:    sprintf(instr,"\tble %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
        case T_LESS_T:    sprintf(instr,"\tblt %s, %s, %s",t0,t1,node->attr_true->attr_string); break;
    }
    add_instruction(instr);
    sprintf(instr,"\tj %s",node->attr_false->attr_string);
    add_instruction(instr);
    current_temp -=2;
    return "";
}
char* Visitor_muncher::munch(ir_const* node){
     if(PRINT_FLUX_MIPS) add_instruction("#CONST-------------------");
     else add_instruction("");

     if(current_temp < 0) current_temp = 0;
     // carrego a constante em um registrador e o retorno
     char instr[MAX_CONST_SIZE];
     sprintf(instr,"\tli %s, %d",registers[current_temp++],node->attr_i);
     add_instruction(instr);
     return registers[current_temp - 1];
}

char* Visitor_muncher::munch(ir_constf* node){
     if(PRINT_FLUX_MIPS) add_instruction("#CONSTF-------------------");
     else add_instruction("");

     if(current_temp < 0) current_temp = 0;
     // Método de arredondamento, não utilizamos os registradores de float
     char instr[MAX_CONST_SIZE];
     sprintf(instr,"\tli %s, %d",registers[current_temp++],(int)node->attr_f);
     add_instruction(instr);
     return registers[current_temp - 1];

}
char* Visitor_muncher::munch(ir_eseq* node){}
char* Visitor_muncher::munch(ir_expr_void* node){}
char* Visitor_muncher::munch(ir_jump* node){
    if(PRINT_FLUX_MIPS) add_instruction("#JUMP-------------------");
    else add_instruction("");

    char instr[MAX_CONST_SIZE];
    //if(current_temp == -1 )current_temp++;
    // const char* t0 = registers[current_temp++];
    //const char* t1 = registers[current_temp--];
    ir_name* name = dynamic_cast<ir_name*>(node->attr_expr);
    if(name){
        sprintf(instr,"\tj %s",name->attr_name->attr_string);
        add_instruction(instr);
    }
    //current_temp--;
    return "";
}
char* Visitor_muncher::munch(ir_label* node){
    char instr[MAX_CONST_SIZE];
    sprintf(instr,"%s:",node->attr_n->attr_string);
    add_instruction(instr);
    return "";
}
char* Visitor_muncher::munch(ir_mem* node){
    if(PRINT_FLUX_MIPS) add_instruction("#MEM-------------------");
    else add_instruction("");

    if(current_temp < 0 )current_temp = 0;
    char instr[MAX_CONST_SIZE];
    char* dest = registers[current_temp++];
    int offset = 0 ;
    char* t0 = NULL;
    ir_binop* binop = dynamic_cast<ir_binop*>(node->attr_expr);
    if(binop){
        ir_const* const1 = dynamic_cast<ir_const*>(binop->attr_exp1);
        ir_const* const2 = dynamic_cast<ir_const*>(binop->attr_exp2);
        if(const1){
            offset = (binop->attr_op == T_MINUS)?(-1)*const1->attr_i:const1->attr_i;
            ir_mem* mem_inter = dynamic_cast<ir_mem*>(binop->attr_exp2);
            if(mem_inter)  t0 = this->munch(mem_inter);
            else t0 = this->munch(binop->attr_exp2);
        }
        else if(const2){
            offset = (binop->attr_op == T_MINUS)?(-1)*const2->attr_i:const2->attr_i;
            ir_mem* mem_inter = dynamic_cast<ir_mem*>(binop->attr_exp1);
            if(mem_inter)  t0 = this->munch(mem_inter);
            else t0 = this->munch(binop->attr_exp1);
        }else{
            t0 = this->munch(binop);
            dest = t0 ;
        }
    }else add_instruction("#caso não esperado");

    sprintf(instr,"\tlw %s, %d(%s)",dest,offset,t0);
   // current_temp--;
    add_instruction(instr);
    return dest;
}

char* Visitor_muncher::munch(ir_move* node){
    if(PRINT_FLUX_MIPS) add_instruction("#MOVE-------------------");
    //else add_instruction("");

    int offset = 0 ;
    int enter = 0 ;
    char instr[MAX_CONST_SIZE];
    char* t0 = NULL;
    char* t1 = NULL;
    ir_mem* mem1 = dynamic_cast<ir_mem*>(node->attr_expr1);
    ir_mem* mem2 = dynamic_cast<ir_mem*>(node->attr_expr2);
    if(mem1){
        // memoria <- valor    Store word
        ir_binop* binop = dynamic_cast<ir_binop*>(mem1->attr_expr);
        if(binop){
            ir_temp* temp1   = dynamic_cast<ir_temp*>(binop->attr_exp1);
            ir_const* const2 = dynamic_cast<ir_const*>(binop->attr_exp2);
            if(temp1 && const2 ){
                t1 = this->munch(temp1);
                offset = (binop->attr_op == T_MINUS)?(-1)*const2->attr_i:const2->attr_i;
            }else{
                ir_mem* mem1   = dynamic_cast<ir_mem*>(binop->attr_exp1);
                if(mem1 && const2){
                    t1 = this->munch(mem1); // pega endereco do objeto
                    offset = (binop->attr_op == T_MINUS)?(-1)*const2->attr_i:const2->attr_i;
                    current_temp++;
                    enter = 2 ;
                }else{
                    t1 = this->munch(binop);
                    current_temp++;
                    enter = 2 ;
                }
            }
            if(current_temp < 0) current_temp = 0 ; // caso ele decremente sem usar um registrador callee
            t0 = this->munch(node->attr_expr2);
            sprintf(instr,"\tsw %s, %d(%s)",t0,offset,t1);
            add_instruction(instr);
            current_temp--;
            current_temp-= enter;
            if(current_temp < 0) current_temp = 0 ; // caso ele decremente sem usar um registrador callee
        }
    }else{
        t0 = this->munch(node->attr_expr1);
        t1 = this->munch(node->attr_expr2);
        sprintf(instr,"\tmove $t9, %s",t1);
        add_instruction(instr);
        current_temp--;
        ir_temp* temp_retorno = dynamic_cast<ir_temp*>(node->attr_expr1);
        if(temp_retorno){
            // Caso onde o move está relacionado ao retorno do método
            if(this->current_frame->attr_temp == temp_retorno->attr_temp){
                add_instruction("\tmove $v0, $t9");
                add_instruction("\tlw $ra, 0($sp)");
                add_instruction("\tjr $ra");
                current_temp--;
            }
        }
    }
    return t0;
}

char* Visitor_muncher::munch(ir_name* node){
    char instr[MAX_CONST_SIZE];
    char* t0 = registers[current_temp++];
    sprintf(instr,"\tla %s, str%d",t0,node->attr_name->attr_id);
    add_instruction(instr);
    current_temp--;
    return registers[current_temp];
}
char* Visitor_muncher::munch(ir_seq* node){
     int first = 0;
     if(this->bocejo_helper == 0 ){
        this->bocejo_helper = 1;
        first = 1;
     }

     if(typeid(*(node->attr_stmt1)) == typeid(ir_seq)) this->munch((ir_seq*)node->attr_stmt1);
     else this->munch(node->attr_stmt1);

     if(typeid(*(node->attr_stmt2)) == typeid(ir_seq)) this->munch((ir_seq*)node->attr_stmt2);
     else this->munch(node->attr_stmt2);

     if(first) add_instruction("\tmove $t0, $t9");
     return "$t0";
}

char* Visitor_muncher::munch(ir_temp* node){
    if(strcmp(node->attr_temp->attr_string,"fp") == 0 ) return "$fp";
    else if(strcmp(node->attr_temp->attr_string,"sp") == 0 ) return "$sp";
    else if(strcmp(node->attr_temp->attr_string,"zero") == 0 ) return "$zero";
    if(current_temp < 0) current_temp =0;
    return registers[current_temp++];
}

char* Visitor_muncher::munch(ir_params* node){
    this->munch(node->attr_sl);
    return  registers[dest_temp];
}

char* Visitor_muncher::munch(ir_expr* node){
    ir_binop* binop   = dynamic_cast<ir_binop*>(node);
    ir_call* call     = dynamic_cast<ir_call*>(node);
    ir_const* integer = dynamic_cast<ir_const*>(node);
    ir_constf* real   = dynamic_cast<ir_constf*>(node);
    ir_name* name     = dynamic_cast<ir_name*>(node);
    ir_temp* temp     = dynamic_cast<ir_temp*>(node);
    ir_mem* mem       = dynamic_cast<ir_mem*>(node);
    ir_params* params = dynamic_cast<ir_params*>(node);

    ir_cjump* cjump         = dynamic_cast<ir_cjump*>(node);
    ir_expr_void* expr_void = dynamic_cast<ir_expr_void*>(node);
    ir_jump* jump           = dynamic_cast<ir_jump*>(node);
    ir_label* label         = dynamic_cast<ir_label*>(node);
    ir_move* move           = dynamic_cast<ir_move*>(node);


    if(cjump) return this->munch(cjump);
    if(jump) return this->munch(jump);
    if(expr_void) return this->munch(expr_void);
    if(label) return this->munch(label);
    if(move) return this->munch(move);



    if(binop) return this->munch(binop);
    if(call) return this->munch(call);
    if(integer) return this->munch(integer);
    if(real) return this->munch(real);
    if(mem) return this->munch(mem);
    if(name) return this->munch(name);
    if(temp) return this->munch(temp);
    if(params) return this->munch(params);
    if(typeid(*node)==typeid(ir_seq)) return this->munch((ir_seq*)node);
    if(typeid(*node)==typeid(ir_stmt_list)){
        ir_stmt_list* sl = (ir_stmt_list*)node;
        while(sl){
            if(sl->attr_stmt) this->munch((ir_expr*)sl->attr_stmt);
            if(sl->attr_stmt_list) this->munch((ir_expr*)sl->attr_stmt_list);
            sl = sl->attr_stmt_list;
        }
        return registers[dest_temp];
    }
    return "erro expr not found";
}

char* Visitor_muncher::munch(ir_stmt* node){
    ir_cjump* cjump         = dynamic_cast<ir_cjump*>(node);
    ir_expr_void* expr_void = dynamic_cast<ir_expr_void*>(node);
    ir_jump* jump           = dynamic_cast<ir_jump*>(node);
    ir_label* label         = dynamic_cast<ir_label*>(node);
    ir_move* move           = dynamic_cast<ir_move*>(node);


    if(cjump) return this->munch(cjump);
    if(jump) return this->munch(jump);
    if(expr_void) return this->munch(expr_void);
    if(label) return this->munch(label);
    if(move) return this->munch(move);
    return "erro stmt not found ";
}

ir_expr* Visitor_muncher::visit(ir_binop* node){ return NULL; }
ir_expr* Visitor_muncher::visit(ir_call* node)
{
    char instr[MAX_CONST_SIZE];
    if(strcmp(node->attr_name->attr_name->attr_string,"PRINTLN") == 0){
        if(PRINT_FLUX_MIPS) add_instruction("#PRINTLN-------------------");
        else add_instruction("");

        ir_expr_list* list = node->attr_el;
        while(list != NULL){
            this->bocejo_helper = 0;
            ir_name* name = dynamic_cast<ir_name*>(list->attr_expr);
            if(name){
                add_instruction("\tli $v0, 0x04");
                sprintf(instr,"\tla $a0, str%d",name->attr_name->attr_id);
                add_instruction(instr);
            }
            else{
                current_temp = 0 ; // usaremos o temporario t0 como retorno
                dest_temp = -1 ; // temporario de destino usados em caso de Cjump
//                sprintf(instr,"\tli %s, 0",registers[current_temp]); // zera o registrador que terá o retorno
//                add_instruction(instr);
                sprintf(instr,"\tmove $a0, %s",this->munch(list->attr_expr));
                add_instruction(instr);
                add_instruction("\tli $v0, 0x01");
            }
            add_instruction("\tsyscall");
            list = list->attr_expr_list;
        }
    }else{
        if(strcmp(node->attr_name->attr_name->attr_string,"READ") == 0){
            ir_expr_list* list = node->attr_el;
            add_instruction("\tli $v0, 5");
            add_instruction("\tsyscall");
            ir_mem* mem = dynamic_cast<ir_mem*>(list->attr_expr);
            char* t0 = NULL;
            if(mem) t0 = this->munch(mem->attr_expr);
            sprintf(instr,"\tsw $v0, 0(%s)",t0);
            add_instruction(instr);
        }else this->munch(node);
    }
    this->bocejo_helper = 0;
}

ir_stmt* Visitor_muncher::visit(ir_cjump* node){
    this->munch(node);
    return NULL;
}
ir_expr* Visitor_muncher::visit(ir_const* node){
    return node;
}

ir_expr* Visitor_muncher::visit(ir_constf* node){
    return node;
}

ir_expr* Visitor_muncher::visit(ir_eseq* node){
    return node;
}

ir_stmt* Visitor_muncher::visit(ir_expr_void* node){
    if(node->attr_expr) node->attr_expr->accept(this);
    return NULL;
}

ir_stmt* Visitor_muncher::visit(ir_jump* node){
    this->munch(node);
    return node;
}

ir_stmt* Visitor_muncher::visit(ir_label* node){
    this->munch(node);
    return NULL;
}
ir_expr* Visitor_muncher::visit(ir_mem* node){
    return node;
}
ir_stmt* Visitor_muncher::visit(ir_move* node){
    this->munch(node);
    return NULL;
}
ir_expr* Visitor_muncher::visit(ir_name* node){
    return node;
}
ir_stmt* Visitor_muncher::visit(ir_seq* node){

    dest_temp = -1;
    this->munch(node->attr_stmt1);
    this->munch(node->attr_stmt2);
    return NULL;
}
ir_expr* Visitor_muncher::visit(ir_temp* node)
{
    return node;
}
ir_expr* Visitor_muncher::visit(ir_expr_list* node){
    return node; }
ir_stmt_list* Visitor_muncher::visit(ir_stmt_list* node)
{
    this->bocejo_helper = 1 ; // marcar que estamos n
    if(node->attr_stmt) node->attr_stmt->accept(this);
    if(node->attr_stmt_list) node->attr_stmt_list->accept(this);
    return node;
}

ir_expr* Visitor_muncher::visit(ir_params* node){
    return node;
}


/**
* Visit responsável por canonizar e liearizar as Representações intermediárias
*/
Visitor_Canonic::Visitor_Canonic()
{
    has_change = 1;
}

//ir_expr* Visitor_Canonic::remove_seq(ir_expr_list* node){
//
//}

ir_stmt* Visitor_Canonic::remove_seq(ir_stmt* node)
{
    if(node == NULL) return NULL;
    if(typeid(*node) == typeid(ir_seq)){
        ir_seq* s = (ir_seq*) node;
        return new ir_stmt_list(remove_seq(s->attr_stmt1), (ir_stmt_list*)remove_seq(s->attr_stmt2));
    }
    return new ir_stmt_list(node,NULL);
}

ir_stmt_list* Visitor_Canonic::remove_seq(ir_stmt_list* node)
{
    if(node != NULL)
    {
        if(node->attr_stmt != NULL && typeid(*(node->attr_stmt)) == typeid(ir_seq))
        {
            ir_stmt_list* sl = (ir_stmt_list*)remove_seq(node->attr_stmt);
            ir_stmt_list* last = sl;
            while(last->attr_stmt_list != NULL) last = last->attr_stmt_list;
            if(node->attr_stmt_list) last->attr_stmt_list = remove_seq(node->attr_stmt_list);
            return sl;
        }//else{
//            ir_expr_void* expr_void = dynamic_cast<ir_expr_void*>(node->attr_stmt);
//            if(expr_void){
//                ir_call* call = (ir_call*)expr_void->attr_expr;
//                ir_expr_list* aux_e = call->attr_el;
//                call->attr_params = (aux_e)?new ir_stmt_list(NULL,NULL):NULL;
//                ir_stmt_list* aux_s = call->attr_params;
//                while(aux_e){
//                    aux_s->attr_stmt = remove_seq((ir_stmt*)aux_e->attr_expr);
//                    aux_e = aux_e->attr_expr_list;
//                    if(aux_e){
//                        aux_s->attr_stmt_list = new ir_stmt_list(NULL,NULL);
//                        aux_s = aux_s->attr_stmt_list;
//                    }
//                }
//                call->attr_el = NULL;
//                expr_void->attr_expr = call;
//            }
//        }
        if(node->attr_stmt_list) node->attr_stmt_list = remove_seq(node->attr_stmt_list);
    }
    return node;
}


ir_stmt_list* Visitor_Canonic::visit(ir_stmt_list* node)
{
    ir_stmt* stmt = NULL;
    ir_stmt_list* stmt_list = NULL;
    if(node->attr_stmt) stmt = node->attr_stmt->accept(this);
    if(node->attr_stmt_list) stmt_list = node->attr_stmt_list->accept(this);
    node->attr_stmt = stmt;
    node->attr_stmt_list = stmt_list;
    return node;
}

ir_expr* Visitor_Canonic::visit(ir_binop* node)
{

    ir_expr* expr1 = node->attr_exp1;
    ir_expr* expr2 = node->attr_exp2;
    if(node != NULL)
    {
        if( typeid(*(node->attr_exp1)) == typeid(ir_eseq) ){
            has_change = 1;
            ir_temp* t = new ir_temp(new temp());
            ir_move* mv = new ir_move(t,node->attr_exp2);
            node->attr_exp2 = t ;
            ir_eseq* aux = (ir_eseq*)node->attr_exp1;
            node->attr_exp1 = ((ir_eseq*)node->attr_exp1)->attr_expr;
            aux->attr_expr = node;
            return new ir_eseq(mv,aux);
        }else expr1 = node->attr_exp1->accept(this);

        if( typeid(*(node->attr_exp2)) == typeid(ir_eseq) ){
            has_change = 1;
            ir_temp* t = new ir_temp(new temp());
            ir_move* mv = new ir_move(t,node->attr_exp1);
            node->attr_exp1 = t ;
            ir_eseq* aux = (ir_eseq*)node->attr_exp2;
            node->attr_exp2 = ((ir_eseq*)node->attr_exp2)->attr_expr;
            aux->attr_expr = node;
            return new ir_eseq(mv,aux);
        }else expr2 = node->attr_exp2->accept(this);

        // canonizando um caso extra, soma de zeros retorna a constante zero
        if((typeid(*(node->attr_exp1)) == typeid(ir_const))&&(typeid(*(node->attr_exp2)) == typeid(ir_const))){
            if(((ir_const*)node->attr_exp1)->attr_i == 0 && ((ir_const*)node->attr_exp2)->attr_i == 0)
                return new ir_const(0);
        }
    }
    return new ir_binop(node->attr_op,expr1,expr2);
}
ir_expr* Visitor_Canonic::visit(ir_call* node)
{
    ir_eseq* eseq = dynamic_cast<ir_eseq*>(node->attr_el->attr_expr);
    if(eseq){
        has_change = 1;
        node->attr_el->attr_expr = eseq->attr_expr;
        eseq->attr_expr = node;
        return eseq;
    }
    node->attr_el = (ir_expr_list*)node->attr_el->accept(this);
    return node;
}
ir_stmt* Visitor_Canonic::visit(ir_cjump* node)
{

    ir_expr* expr1 = node->attr_expr1;
    ir_expr* expr2 = node->attr_expr2;
    if (node != NULL)
    {
        if (node->attr_expr1 != NULL){
            if(typeid(*(node->attr_expr1)) == typeid(ir_eseq)){
                has_change =1;
                ir_seq* result = new ir_seq(NULL,NULL);
                result->attr_stmt1 = ((ir_eseq*)node->attr_expr1)->attr_stmt->accept(this);
                node->attr_expr1   = ((ir_eseq*)node->attr_expr1)->attr_expr->accept(this);
                result->attr_stmt2 = node;
                return result;
            }else expr1 = node->attr_expr1->accept(this);
        }

        if(node->attr_expr2 != NULL)
        {
            if(typeid(*(node->attr_expr2)) == typeid(ir_eseq))
            {
                has_change = 1;
                ir_temp* t = new ir_temp(new temp());
                ir_move* mv = new ir_move(t,node->attr_expr1->accept(this));
                node->attr_expr1 = t;
                ir_seq* sq = new ir_seq(((ir_eseq*)node->attr_expr2)->attr_stmt,node);
                node->attr_expr2 = ((ir_eseq*)node->attr_expr2)->attr_expr->accept(this);
                return new ir_seq(mv,sq);
            }
            else expr2 = node->attr_expr2->accept(this);
        }
    }
    return new ir_cjump(node->attr_op,expr1,expr2,node->attr_true,node->attr_false);
}
ir_expr* Visitor_Canonic::visit(ir_const* node)
{
    return node;
}
ir_expr* Visitor_Canonic::visit(ir_constf* node)
{
    return node;
}
ir_expr* Visitor_Canonic::visit(ir_eseq* node)
{
    if (node != NULL){
        if (node->attr_expr != NULL && typeid(*(node->attr_expr)) == typeid(ir_eseq)){
            has_change = 1;
            ir_eseq* eseq = (ir_eseq*) node->attr_expr;
            ir_seq* seq = new ir_seq(node->attr_stmt->accept(this), eseq->attr_stmt->accept(this));
            node->attr_stmt = seq;
            node->attr_expr = eseq->attr_expr->accept(this);
        }
        else{
       //     has_change =1;
       //     return (ir_expr*)node->attr_stmt->accept(this);
        }
    }
    return node;
}
ir_stmt* Visitor_Canonic::visit(ir_expr_void* node){

    ir_eseq* eseq = dynamic_cast<ir_eseq*>(node->attr_expr->accept(this));
    ir_seq* result = new ir_seq(NULL,NULL);
    if(eseq){
        has_change = 1;
        result->attr_stmt1 = eseq->attr_stmt;
        result->attr_stmt2 = node;
        node->attr_expr = eseq->attr_expr;
        return result;
    }else node->attr_expr =  node->attr_expr->accept(this);
    return node;
}
ir_stmt* Visitor_Canonic::visit(ir_jump* node){
    ir_expr* expr = node->attr_expr;
    if (node->attr_expr != NULL)
    {
        if(typeid(*(node->attr_expr)) == typeid(ir_eseq)){
            has_change = 1;
            ir_stmt* stmt   = ((ir_eseq*)node->attr_expr)->attr_stmt->accept(this);
            node->attr_expr = ((ir_eseq*)node->attr_expr)->attr_expr->accept(this);
            return new ir_seq(stmt,node);
        }
        else expr = node->attr_expr->accept(this);
    }
    return new ir_jump(expr,node->attr_labels);
}
ir_stmt* Visitor_Canonic::visit(ir_label* node)
{
    return node;
}
ir_expr* Visitor_Canonic::visit(ir_mem* node)
{

    if(node != NULL)
    {
        if(typeid(*(node->attr_expr)) == typeid(ir_eseq))
        {
            has_change = 1;
            ir_expr* up = node->attr_expr->accept(this);
            node->attr_expr = ((ir_eseq*)up)->attr_expr->accept(this);
            ((ir_eseq*)up)->attr_expr = node;
            return up;
        }else node->attr_expr = node->attr_expr->accept(this);
    }
    return node;
}
ir_stmt* Visitor_Canonic::visit(ir_move* node)
{

    ir_expr* expr1 = node->attr_expr1;
    ir_expr* expr2 = node->attr_expr2;
    if(node != NULL)
    {
        if(node->attr_expr2 != NULL)
        {
            if(typeid(*(node->attr_expr2)) == typeid(ir_eseq))
            {
                has_change = 1;
                ir_expr* eseq = node->attr_expr2->accept(this);
//                 ir_move(e2,ir_eseq(s,e1)) - > ir_seq(s,ir_move(e2,e1))
                ir_seq* result = new ir_seq(NULL,NULL);
                result->attr_stmt1 = ((ir_eseq*)eseq)->attr_stmt;
                result->attr_stmt2 = node;
                node->attr_expr2   = ((ir_eseq*)eseq)->attr_expr;
                return result;
            }else{
                expr1 = node->attr_expr1->accept(this);
                expr2 = node->attr_expr2->accept(this);
            }
        }
    }
    return new ir_move(expr1,expr2);
}
ir_expr* Visitor_Canonic::visit(ir_name* node)
{
    return node;
}

ir_stmt* Visitor_Canonic::visit(ir_seq* node)
{
    ir_stmt* stmt1 = NULL;
    ir_stmt* stmt2 = NULL;

    if(node->attr_stmt1) stmt1 = node->attr_stmt1->accept(this);
    if(node->attr_stmt2) stmt2 = node->attr_stmt2->accept(this);

    return new ir_seq(stmt1,stmt2);
}
ir_expr* Visitor_Canonic::visit(ir_temp* node)
{
    return node;
}
ir_expr* Visitor_Canonic::visit(ir_expr_list* node)
{
    if(node->attr_expr)      node->attr_expr = node->attr_expr->accept(this);
    if(node->attr_expr_list) node->attr_expr_list = (ir_expr_list*) node->attr_expr_list->accept(this);
    else node->attr_expr;
    return node;
}

ir_expr* Visitor_Canonic::visit(ir_params* node){
    node->attr_sl = node->attr_sl->accept(this);
}
/**
* Visitor que trata a impressao da ri
*
*/

Visitor_print_ir::Visitor_print_ir()
{
    this->level = 0; // nivel raiz da árvore
}

void Visitor_print_ir::identation()
{
    for(int i=1; i <= level; i++) fprintf(stdout,". "); //Define o tamanho da identação
}

ir_stmt_list* Visitor_print_ir::visit(ir_stmt_list* node)
{

    if(node->attr_stmt)
    {
        //fprintf(stdout,"\n");
        node->attr_stmt->accept(this);
    }
    if(node->attr_stmt_list) node->attr_stmt_list->accept(this);
}

ir_expr* Visitor_print_ir::visit(ir_expr_list* node)
{
    if(node != NULL)
    {
        fprintf(stdout,"EXPR_LIST(\n");
        level++;
        this->identation();
        if(node->attr_expr)
        {
            node->attr_expr->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if(node->attr_expr_list)
        {
            node->attr_expr_list->accept(this);
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
}
ir_expr* Visitor_print_ir::visit(ir_call* node)
{
    if(node!=NULL)
    {
        fprintf(stdout,"CALL(\n");
        level++;
        this->identation();
        if(node->attr_name)
        {
            node->attr_name->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if(node->attr_el){
            node->attr_el->accept(this);
        }else{
            if(node->attr_params) node->attr_params->accept(this);
            else fprintf(stdout,"NULL");
        }
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
}
ir_stmt* Visitor_print_ir::visit(ir_cjump* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"CJUMP(\n");
        level++;
        this->identation();
        fprintf(stdout,GET_token_name(node->attr_op));
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_expr1 != NULL)
        {
            node->attr_expr1->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_expr2 != NULL)
        {
            node->attr_expr2->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_true != NULL)
        {
            fprintf(stdout,node->attr_true->toString());
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_false != NULL)
        {
            fprintf(stdout,node->attr_false->toString());
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
}
ir_expr* Visitor_print_ir::visit(ir_const* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"CONST(");
        fprintf(stdout,"%i",node->attr_i);
        fprintf(stdout,")");
    }
}

ir_expr* Visitor_print_ir::visit(ir_constf* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"CONSTF(");
        fprintf(stdout,"%f",node->attr_f);
        fprintf(stdout,")");
    }
}
ir_expr* Visitor_print_ir::visit(ir_name* node)
{
    if (node != NULL)
    {
        if (node->attr_name != NULL)
        {
            fprintf(stdout,"NAME('");
            fprintf(stdout,node->attr_name->toString());
            fprintf(stdout,"')");
        }
    }
}
ir_expr* Visitor_print_ir::visit(ir_temp* node)
{
    if (node != NULL)
    {
        if (node->attr_temp != NULL)
        {
            fprintf(stdout,"TEMP('");
            fprintf(stdout,node->attr_temp->toString());
            fprintf(stdout,"')");
        }
    }
}
ir_expr* Visitor_print_ir::visit(ir_binop* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"BINOP(\n");
        level++;
        this->identation();
        fprintf(stdout,GET_token_name(node->attr_op));
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_exp1 != NULL)
        {
            node->attr_exp1->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_exp2 != NULL)
        {
            node->attr_exp2->accept(this);
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
    return node;
}
ir_stmt* Visitor_print_ir::visit(ir_expr_void* node)
{
    if (node != NULL)
    {
        if (node->attr_expr != NULL)
        {
            level++;
            this->identation();
            fprintf(stdout,"EXP(");
            node->attr_expr->accept(this);
            fprintf(stdout,")");
            level--;
        }
    }
    return node;
}
ir_stmt* Visitor_print_ir::visit(ir_jump* node)
{
    LabelList* tail;
    if (node != NULL)
    {
        fprintf(stdout,"JUMP(\n");
        level++;
        this->identation();
        if (node->attr_expr != NULL)
        {
            node->attr_expr->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,"\n");
        this->identation();
        if (node->attr_labels != NULL)
        {
            fprintf(stdout,node->attr_labels->attr_label->toString());
            // fprintf(stdout,"->");
            tail = node->attr_labels->attr_next;
            while (node->attr_labels->attr_next != NULL)
            {
                fprintf(stdout,tail->attr_label->toString());
                fprintf(stdout,"->");
                tail = tail->attr_next;
            }
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
    return node;
}
ir_stmt* Visitor_print_ir::visit(ir_label* node)
{
    if (node != NULL)
    {
        if (node->attr_n != NULL)
        {
            fprintf(stdout,"\nLABEL('");
            fprintf(stdout,node->attr_n->toString());
            fprintf(stdout,"')\n");

        }
    }
    return node;
}
ir_expr* Visitor_print_ir::visit(ir_mem* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"MEM(\n");
        level++;
        this->identation();
        if (node->attr_expr != NULL)
        {
            node->attr_expr->accept(this);
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
    return node;
}

//EXTENDS STMT
ir_stmt* Visitor_print_ir::visit(ir_move* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"MOVE(\n");
        level++;
        this->identation();
        if (node->attr_expr1 != NULL)
        {
            node->attr_expr1->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");
        this->identation();
        if (node->attr_expr2 != NULL)
        {
            node->attr_expr2->accept(this);
        }
        else fprintf(stdout,"NULL\n");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");
    }
    return node;
}
ir_stmt* Visitor_print_ir::visit(ir_seq* node)
{
    if (node != NULL)
    {

        fprintf(stdout,"SEQ(\n");
        level++;
        this->identation();
        if (node->attr_stmt1 != NULL)
        {
            node->attr_stmt1->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");

        this->identation();
        if (node->attr_stmt2 != NULL)
        {
            node->attr_stmt2->accept(this);
        }
        else fprintf(stdout,"NULL");

        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");

    }
    return node;
}

ir_expr* Visitor_print_ir::visit(ir_eseq* node)
{
    if (node != NULL)
    {
        fprintf(stdout,"ESEQ(\n");
        level++;
        this->identation();
        if (node->attr_stmt != NULL)
        {
            node->attr_stmt->accept(this);
        }
        else fprintf(stdout,"NULL");
        fprintf(stdout,",\n");

        this->identation();
        if (node->attr_expr != NULL)
        {
            node->attr_expr->accept(this);
        }
        else fprintf(stdout,"NULL");
        level--;
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,")");

    }
    return node;
}

ir_expr* Visitor_print_ir::visit(ir_params* node){
    node->attr_sl->accept(this);
    return node;
}
