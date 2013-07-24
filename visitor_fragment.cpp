/**
* visitor_fragment.cpp
* @author Claudson Oliveira
* @date   21 de junho de 2011
*
* Arquivo que implementa todo o módulo relacionado os fragmentos
*
**/
#include "visitors.h"
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <string.h>
#include "lexical.h"
#include "semantic.h"
#include "error.h"
#include "symbols_tables.h"
#include "abstract_syntax_tree.h"
#include "visitor_translateAsaIR.h"
#include "visitor_fragment.h"
#include "visitor_ir.h"
#include "mips.h"

/*
* Visitor que imprime os fragmentos das RI
*
*/

Visitor_print_fragment::Visitor_print_fragment(){
    this->level = 0; // nivel raiz da árvore
    fprintf(stdout,"****FRAGMENTS****\n\n");
}

void Visitor_print_fragment::identation(){
    for(int i=1;i < level;i++) fprintf(stdout,"    ");
}

Fragment* Visitor_print_fragment::visit(Procedure* fragment){
    if (fragment != NULL){
        level++;
        fprintf(stdout,"\n............................PROCEDURE...........................\n");
        this->identation();
        FrameMIPS* frame = (FrameMIPS*) fragment->attr_frame;
        fprintf(stdout,"--FRAME:\n");
        level++;
        this->identation();
        fprintf(stdout,"Label:");
        fprintf(stdout,"%s",frame->attr_label->toString());
        fprintf(stdout,"\n");
        this->identation();
        fprintf(stdout,"Temp: ");
        fprintf(stdout,"%s",frame->attr_temp->toString());
        fprintf(stdout,"\n");
        fprintf(stdout,"\n");
        //IMPRIMIR LISTA DE ACESSO?

        if (fragment->attr_body != NULL){
            ir_stmt_list* stmt = fragment->attr_body;
            Visitor_print_ir* print_ir = new Visitor_print_ir();
            Visitor_Canonic* canonic_ir = new Visitor_Canonic();

            // enquanto nenhuma mudanção (has_change) for realizada na RI
            // não pare de canonizar
            //print_ir->visit(stmt);
            fprintf(stdout,"\n\nBODY: \n");
            while(canonic_ir->has_change){
                canonic_ir->has_change = 0;
                stmt = canonic_ir->visit(stmt);
            }
            stmt = canonic_ir->remove_seq(stmt);
            fragment->attr_body = stmt;
            print_ir->visit(stmt);
            fprintf(stdout,"\n");
        }
        level-=2;
        if(fragment->attr_next != NULL)  fragment->attr_next->accept(this);
    }
    return fragment;
}

void Visitor_print_fragment::visit(Literal* literal){
    if (literal != NULL){
        level++;
        fprintf(stdout,"\n.............................LITERAL...........................\n");
        this->identation();
        fprintf(stdout,"Label:");
        fprintf(stdout,literal->attr_string);
        fprintf(stdout,"\n");
        this->identation();
        level--;
        if(literal->attr_next != NULL)  literal->attr_next->accept(this);
    }
}

/*
* Visitor que visita os fragmentos chamando o "muncher" a fim de gerar o código mips
*
*/

Visitor_mips::Visitor_mips(label* label_main){
    this->muncher = new Visitor_muncher();
    this->label_main = label_main;
}

Fragment* Visitor_mips::visit(Procedure* fragment){
    if(fragment != NULL){
        char instr[MAX_CONST_SIZE];
        FrameMIPS* frame = (FrameMIPS*) fragment->attr_frame;
        if(fragment->attr_body != NULL){
            add_instruction("");
            add_instruction("\t.text");
            if(frame->attr_label == this->label_main){
                sprintf(instr,"\t.globl main");
                add_instruction(instr);
                sprintf(instr,"main:");
                add_instruction(instr);
            }else{
                sprintf(instr,"\t.globl %s",frame->attr_label->attr_string);
                add_instruction(instr);
                sprintf(instr,"%s:",frame->attr_label->attr_string);
                add_instruction(instr);
            }
            // ----------------- PRÓLOGO --------------------------
            // aloca o tamanho do frame pelo numero de variaveis usadas
            // assim como
            int size = frame->attr_size + FrameMIPS::wordSize*NUM_REG_PROLOGO ; // (frame->attr_size <= 32 )?32:frame->attr_size;
            sprintf(instr,"# -------------- Prólogo de %s -----------------",frame->attr_label->attr_string);
            add_instruction(instr);
            sprintf(instr,"\tsubu   $sp, $sp, %d",size);
            add_instruction(instr);
            add_instruction("\tsw     $ra, 0($sp)");
            add_instruction("\tsw     $fp, 4($sp)");
            sprintf(instr,"\taddiu   $fp, $sp, %d",size - 4);
            add_instruction(instr);

            ListaAcesso* list_params = frame->attr_ListaAcessoParam;
            int offset = 0;
            int reg_number = 0;
            while(list_params != NULL){
                sprintf(instr,"\tsw $a%d, %d($fp)",reg_number,offset);
                add_instruction(instr);
                offset -= 4;
                list_params = list_params->attr_next;
                reg_number++;
                if(reg_number == 4) break; // mexer no semantico para nao permitir mais de 4 parametros  (usar $s7 para this)
            }
            this->muncher->current_frame = frame;
            this->muncher->visit(fragment->attr_body);
        }
        if(fragment->attr_next != NULL) fragment->attr_next->accept(this);
    }
    return fragment;
}

void Visitor_mips::visit(Literal* literal){

    add_instruction("\t.rdata");
    char instr[MAX_CONST_SIZE];
    sprintf(instr,"str%d:",literal->attr_label->attr_id);
    add_instruction(instr);
    sprintf(instr,"\t.asciiz %s #reserva espaco para o literal",literal->attr_string);
    add_instruction(instr);

    if(literal->attr_next != NULL)  literal->attr_next->accept(this);
}
