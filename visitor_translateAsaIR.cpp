#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <string.h>

#include "visitor_translateAsaIR.h"
#include "abstract_syntax_tree.h"
#include "lexical.h"
#include "visitor_fragment.h"
#include "IR.h"


Visitor_Translate_IR::Visitor_Translate_IR(){
    table_class      = GET_table_class();
    current_class    = NULL;

    first_fragment   = NULL;
    last_fragment    = NULL;
    current_fragment = NULL;
    current_frame    = NULL;
    stmtGERAL        = NULL;
    exprGERAL        = NULL;
    exprNAME_LIST    = NULL;
    current_class_NAME_LIST = NULL;
    label_main = NULL;
}

void Visitor_Translate_IR::visit(node_program* node){
    node->attr_class_list->accept(this);
}
void Visitor_Translate_IR::visit(node_class_list* node){
    current_class = (Symbol_class*)node->attr_id;
    FrameMIPS* frame     = new FrameMIPS(new label(table_class->get_lexeme(current_class)), new temp(), NULL,NULL);
    Procedure* f         = new Procedure(frame,NULL);

    current_fragment   = f;

    //Percorre a ASA setando offsets das variaveis;
    node_var_list* var_list = node->attr_var_list;
    int offset = 0;
    int numAtributo =0;

    if(current_class->father != NULL) offset += current_class->father->offset;

    while (var_list != NULL){
        Symbol_variable* id = (Symbol_variable*)var_list->attr_name->attr_id;
        node_assign* init = var_list->attr_name->attr_assign;
        if (init != NULL){
            if((id->type_object == NULL)&&(id->array==false)){ //primitivo
                init->attr_expr_right->accept(this);
                id->inicializador = exprGERAL;
            }else if(id->array == true){ //vetor
                ((node_new*)init->attr_expr_right)->attr_expr->accept(this);
                id->inicializador = exprGERAL;
            }else{ //objeto
                id->inicializador = new ir_const(1);
            }
        }
        id->is_attribute = 1;
        id->offset = offset;
        offset+=FrameMIPS::wordSize;
        numAtributo++;
        var_list = var_list->attr_var_list;
    }
    //percorre a lista criando um array com as variaveis da classe
    var_list = node->attr_var_list;
    current_class->array_variable = (Symbol_variable**)malloc(sizeof(Symbol_variable*)*numAtributo);
    int posArray = 0;
    while (var_list != NULL){
        Symbol_variable* id = (Symbol_variable*)var_list->attr_name->attr_id;
        current_class->array_variable[posArray++] = id;
        var_list = var_list->attr_var_list;
    }

    current_class->offset        = offset;
    current_class->num_attributo = numAtributo;
    if(first_fragment == NULL){
        first_fragment = f;
        last_fragment  = f;
    }else{
        last_fragment->attr_next = current_fragment;
        last_fragment = current_fragment;
    }

    if(node->attr_method_list != NULL) {
        node->attr_method_list->accept(this);
    }

    if(node->attr_class_list != NULL) node->attr_class_list->accept(this);
}

void Visitor_Translate_IR::visit(node_var_list* node){
    if(node != NULL ){
        ir_stmt* seq1;
        ir_stmt* seq2;
        if(node->attr_name      != NULL){
            node->attr_name->accept(this);
            seq1 = stmtGERAL;
        }
        if(node->attr_var_list  != NULL){
            node->attr_var_list->accept(this);
            seq2 = stmtGERAL;
        }else{
            seq2 = NULL;
        }

        if((seq1 == NULL)&&(seq2 == NULL)){//os 2 são NULL
            stmtGERAL = NULL;
        }else if(seq1 != NULL){// primeiro nao NULL
            if(seq2 != NULL){ //os 2 diferentes
                stmtGERAL = new ir_seq(seq1,seq2);
            }else{ // primeiro tem e segundo NULL
                stmtGERAL = seq1;
            }
        }else{ //primeiro NULL e segundo nao
            stmtGERAL = seq2;
        }
    }
}
void Visitor_Translate_IR::visit(node_method_list* node){
    if(node != NULL ){
        Symbol_method* metodo = (Symbol_method*)node->attr_id;
        char* metodo_name     =  current_class->current_scope->table_method->get_lexeme(metodo);
        char* class_name      =  table_class->get_lexeme(current_class);
        char* params_name     =  NULL;
        int length_params_name = 0 ;
        Symbol_variable* var = NULL;
        for(int i=0;i < ((Symbol_method*)node->attr_id)->num_params;i++ ){
            var = ((Symbol_method*)node->attr_id)->params[i];
            if(var->type_primitive != 0) length_params_name++;
            else length_params_name += strlen(table_class->get_lexeme(var->type_object));
        }
        length_params_name += ((Symbol_method*)node->attr_id)->num_params; // underline entre parametros


        char* label_name    = (char*)malloc(sizeof(char)*(strlen(class_name)+strlen(metodo_name) + length_params_name +2));

        strcpy(label_name,class_name);
        strcat(label_name,"_");
        strcat(label_name,metodo_name);
        var = NULL;
        for(int i=0;i < ((Symbol_method*)node->attr_id)->num_params;i++ ){
            var = ((Symbol_method*)node->attr_id)->params[i];
            if(var->type_primitive != 0){
                switch(var->type_primitive){
                    case T_INT:   strcat(label_name,"_i"); break;
                    case T_FLOAT: strcat(label_name,"_f"); break;
                    case T_BOOLEAN: strcat(label_name,"_b"); break;
                }
            }else{
                strcat(label_name,"_");
                strcat(label_name,table_class->get_lexeme(var->type_object));
            }
        }
        label* lb = new label(label_name);
        if(((Symbol_method*)node->attr_id)->return_primitive == T_VOID) this->label_main = lb;

        FrameMIPS* frame = new FrameMIPS(lb, new temp(), new ListaAcesso(NULL,NULL),new ListaAcesso(NULL,NULL));
        Procedure* f     = new Procedure(frame,NULL);

        current_fragment = f;
        current_frame    = frame;

        frame->attr_this = frame->adicionaParam(true,4); //Cria o this;
        ListaAcesso* aux = frame->attr_ListaAcessoParam;
        aux->attr_l = frame->attr_this;//Passa-o como primeiro atributo

        if(node->attr_param_list   != NULL){
            node_var_list* params = node->attr_param_list;
            while(params != NULL){
                node_name_decl* decla = params->attr_name;
                Symbol_variable* id = (Symbol_variable*)decla->attr_id;
                AcessoLocal* a      = NULL;
                a = current_frame->adicionaParam(true,4);
                id->acesso = a;
                aux->attr_next = new ListaAcesso(a,NULL);
                aux = aux->attr_next;
                params = params->attr_var_list;
            }
        }
        if(node->attr_stmt_list   != NULL){
            node->attr_stmt_list->accept(this);
            f->attr_body = new ir_stmt_list(stmtGERAL,NULL);
        }
        last_fragment->attr_next = current_fragment;
        last_fragment = current_fragment;
        if(node->attr_method_list != NULL) node->attr_method_list->accept(this);
    }
}

void Visitor_Translate_IR::visit(node_assign* node){
    node->attr_expr_left->accept(this);
    ir_expr* exp_left  = exprGERAL;
    ir_expr* exp_right = NULL;

    if(node->type_assing == node_assign::INIT_ARRAY){
        ir_move* alocacao = NULL;
        node_new* no_new = (node_new*)node->attr_expr_right;
        //Se puder alocar em tempo de compilação, coloca no Frame
        if(typeid(*(no_new->attr_expr)) == typeid(node_primitive)){
            int quant = atoi(((node_primitive*)no_new->attr_expr)->attr_value);
            ir_expr_list* el = new ir_expr_list(new ir_const(quant*FrameMIPS::wordSize+4),NULL); //+4 para armazenar o lenght
            alocacao = new ir_move(exp_left,new ir_call(new ir_name(new label("MALLOC")),el));
            //INICIALIZAR O OBJETO;
            ir_seq* inicializacao = initArray(exp_left,quant); //passa sem o lenght
            ir_seq* seq = new ir_seq(alocacao,inicializacao);//primeiro aloca, segundo construtor
            stmtGERAL = seq;
        }else{ //Senão, coloca na heap usando malloc?

        }
    }
    else if(node->type_assing == node_assign::INIT_OBJECT){
        Symbol_class* classe = ((Symbol_class*) ((node_new*) node->attr_expr_right)->attr_info_type);
        //Aloca espaço no Frame
        ir_expr_list* el = new ir_expr_list(new ir_const(classe->offset),NULL);
        ir_move* alocacao = new ir_move(exp_left,new ir_call(new ir_name(new label("MALLOC")),el));
        //INICIALIZAR O OBJETO;
        ir_seq* inicializacao = initObject(exp_left,classe);
        ir_seq* seq = new ir_seq(alocacao,inicializacao);//primeiro aloca, segundo construtor
        stmtGERAL = seq;
    }
    else{
        node->attr_expr_right->accept(this);
        exp_right = exprGERAL;
        stmtGERAL = new ir_move(exp_left,exp_right);
    }
}
void Visitor_Translate_IR::visit(node_primitive* node){
   if (node!=NULL){
       switch(node->attr_token){
           case T_NUM_INT:
               exprGERAL = new ir_const(atoi(node->attr_value));
               break;
           case T_NUM_REAL:
                exprGERAL = new ir_constf(atof(node->attr_value));
                break;
           case T_TRUE:
                exprGERAL = new ir_const(1);
                break;
           case T_FALSE:
                exprGERAL = new ir_const(0);
                break;
           case T_LITERAL:{
                Literal* l = new Literal(node->attr_value);
                last_fragment->attr_next = l;
                last_fragment = l;
                exprGERAL = new ir_name(l->attr_label);
                break;
           }
       }
   }

}
void Visitor_Translate_IR::visit(node_call* node){
    ir_stmt_list* sl = NULL;
    ir_stmt_list* sl_aux = NULL;

    ir_expr_list* el = NULL;
    ir_expr_list* el_aux = NULL;

    Symbol_method* id = (Symbol_method*) node->attr_id;

    if(exprNAME_LIST == NULL){ //Foi chamado sozinho sem this
        exprNAME_LIST = new ir_binop(T_PLUS,NULL,current_frame->attr_this->codigoAcesso());
        current_class_NAME_LIST = current_class;
    }

    char* metodo_name    = NULL;
    Symbol_class* class_ = id->owner;
    // label deve estar associado ao cara que contem o metodo, e pode ser um pai
    // laço descobre quem é essa pessoa.
    do{
        metodo_name = class_->current_scope->table_method->get_lexeme(id);
        if(!metodo_name)class_ = class_->father;
    }while(metodo_name == NULL);

    char* class_name      =  table_class->get_lexeme(class_);
    char* params_name     =  NULL;
    int length_params_name = 0 ;
    Symbol_variable* var = NULL;
    for(int i=0;i < ((Symbol_method*)node->attr_id)->num_params;i++ ){
        var = ((Symbol_method*)node->attr_id)->params[i];
        if(var->type_primitive != 0) length_params_name++;
        else length_params_name += strlen(table_class->get_lexeme(var->type_object));
    }
    length_params_name += ((Symbol_method*)node->attr_id)->num_params; // underline entre parametros
    char* label_name      = (char*)malloc(sizeof(char)*(strlen(class_name)+strlen(metodo_name)+ length_params_name+ 2));

    strcpy(label_name,class_name);
    strcat(label_name,"_");
    strcat(label_name,metodo_name);

    var = NULL;
    for(int i=0;i < ((Symbol_method*)node->attr_id)->num_params;i++ ){
        var = ((Symbol_method*)node->attr_id)->params[i];
        if(var->type_primitive != 0){
            switch(var->type_primitive){
                case T_INT:   strcat(label_name,"_i"); break;
                case T_FLOAT: strcat(label_name,"_f"); break;
                case T_BOOLEAN: strcat(label_name,"_b"); break;
            }
        }else{
            strcat(label_name,"_");
            strcat(label_name,table_class->get_lexeme(var->type_object));
        }
    }

    el = new ir_expr_list(exprNAME_LIST->attr_exp2,NULL);
    el_aux = el;


   node_expr_list* e = node->attr_expr_list;
   while(e != NULL){
        e->attr_expr->accept(this);
        el_aux->attr_expr_list = new ir_expr_list(exprGERAL,NULL);
        el_aux = el_aux->attr_expr_list;
        e = e->attr_expr_list;

    }


    if((id->return_object==NULL)&&(id->array == 0)){ //retorna um primitivo, logo não terá concatenação posteriormente
        exprGERAL     = new ir_call(new ir_name(new label(label_name)),el);
        exprNAME_LIST = NULL; //Seta para sair
    }
    else{//Se for objeto ou array
        if(node->attr_expr != NULL){ //Se tiver continuação
            exprNAME_LIST = new ir_binop(T_PLUS,NULL,new ir_call(new ir_name(new label(label_name)),el));
            current_class_NAME_LIST = id->return_object;
            node->attr_expr->accept(this);
        }
        else{
            exprGERAL     = new ir_call(new ir_name(new label(label_name)),el);
            exprNAME_LIST = NULL;
        }
    }

    stmtGERAL = new ir_expr_void(exprGERAL);

}
void Visitor_Translate_IR::visit(node_if* node){
    if(node != NULL ){

        label* entao = new label();
        label* fim = new label();
        label* fimse = new label();
        label* senao = new label();
        label* novoRotulo = new label();

        if(node->attr_expr) node->attr_expr->accept(this);
        ir_expr* e   = exprGERAL;
        if(node->attr_stmt) node->attr_stmt->accept(this);
        ir_stmt* s1  = stmtGERAL;
        if(node->attr_stmt_else != NULL){
            node->attr_stmt_else->accept(this);
            ir_stmt* s2  = stmtGERAL;
            stmtGERAL = new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e,new ir_const(0),entao,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(senao),new LabelList(senao,NULL)))),new ir_seq(new ir_label(entao),new ir_seq(new ir_seq(s1,new ir_jump(new ir_name(fimse),new LabelList(fimse,NULL))),new ir_seq(new ir_label(senao),new ir_seq(s2,new ir_label(fimse))))));
        }
        else{
            stmtGERAL = new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e,new ir_const(0),entao,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(fim),new LabelList(fim,NULL)))),new ir_seq(new ir_label(entao),new ir_seq(s1,new ir_label(fim))));
        }
    }
}
void Visitor_Translate_IR::visit(node_this* node){
    if (node != NULL){
        exprNAME_LIST = new ir_binop(T_PLUS,NULL,current_frame->attr_this->codigoAcesso());
        current_class_NAME_LIST = current_class;
        if(node->attr_expr) node->attr_expr->accept(this);
        else{
            exprGERAL = current_frame->attr_this->codigoAcesso();
        }
    }
}
void Visitor_Translate_IR::visit(node_name* node){
    if(node != NULL){
        Symbol_variable* id = (Symbol_variable*) node->attr_id;
            if(id->array){
                printf("ABC");
            }
            if((!id->is_attribute)&&(id->type_object == NULL)){ //Variavel primitiva
                exprGERAL = id->acesso->codigoAcesso();
                if((id->array==true)&&(node->attr_expr)) { //vetor e esta acessadno length
                    exprGERAL = new ir_mem(new ir_binop(T_PLUS,id->acesso->codigoAcesso(),new ir_const(0)));
                }
            }
            else{
                if((!id->is_attribute)&&(id->type_object != NULL)) {  //Variavel objeto
                    ir_expr* e1   = id->acesso->codigoAcesso();
                    exprNAME_LIST = new ir_binop(T_PLUS,NULL,e1);
                    current_class_NAME_LIST = id->type_object;

                }
                if(id->is_attribute){  //É Atributo
                    ir_expr* e1;
                    ir_expr* e2;
                    if(exprNAME_LIST==NULL){ //é o primeiro, logo não tem this antes
                        e2 = new ir_binop(T_PLUS,new ir_const(id->offset),current_frame->attr_this->codigoAcesso());
                        e2 = new ir_mem(e2);
                        exprNAME_LIST = new ir_binop(T_PLUS,NULL,e2);
                        current_class_NAME_LIST = id->type_object;
                    }else{ //já foi setado incialmente com this ou é concatenamção
                        e1 = exprNAME_LIST->attr_exp2;
                        e2 = new ir_binop(T_PLUS,new ir_const(id->offset),e1);
                        e2 = new ir_mem(e2);
                        exprNAME_LIST = new ir_binop(T_PLUS,NULL,e2);
                        current_class_NAME_LIST = id->type_object;
                    }
                    if((id->array==true)&&(node->attr_expr)) { //vetor e esta acessadno length
                        exprGERAL = new ir_mem(new ir_binop(T_PLUS,e2,new ir_const(0)));
                        exprNAME_LIST = NULL;
                    }
                }
                if(exprNAME_LIST!=NULL){
                    if(node->attr_expr != NULL) node->attr_expr->accept(this); //se tem próximo, o chama
                    else{
                        exprGERAL = exprNAME_LIST->attr_exp2;
                        exprNAME_LIST = NULL;
                    }
                }
            }
        }
}
void Visitor_Translate_IR::visit(node_name_decl* node){
    if(node != NULL ){
        Symbol_variable* id = (Symbol_variable*)node->attr_id;
        AcessoLocal* a      = NULL;
        a = current_frame->adicionaLocal(true,4);
        id->acesso = a;
        current_frame->attr_ListaAcesso->attr_next = new ListaAcesso(a,NULL);
        if(node->attr_assign != NULL) node->attr_assign->accept(this);
        else stmtGERAL = new ir_move(id->acesso->codigoAcesso(),new ir_temp(FrameMIPS::zero) );
    }
}
void Visitor_Translate_IR::visit(node_op_addition* node){
    //Add,Minus
    if(node!= NULL){
        int op = node->attr_operator;
        node->attr_expr_1->accept(this);
        ir_expr* e1 = exprGERAL;
        node->attr_expr_2->accept(this);
        ir_expr* e2 = exprGERAL;
        exprGERAL = new ir_binop(op,e1,e2);
    }
}
void Visitor_Translate_IR::visit(node_op_boolean* node){
    //And, Or
    if(node != NULL ){
        int op      = node->attr_operator;
        node->attr_expr_1->accept(this);
        ir_expr* e1 = exprGERAL;
        node->attr_expr_2->accept(this);
        ir_expr* e2 = exprGERAL;

        AcessoLocal* a = current_frame->adicionaLocal(true,4);
        label* l1 = new label();
        label* l2 = new label();
        label* l3 = new label();
        label* novoRotulo = new label();
        label* novoRotulo2 = new label();


        if (op == T_AND){
            exprGERAL = new ir_eseq(new ir_seq(new ir_move(a->codigoAcesso(),new ir_const(0)),new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e1,new ir_const(0),l1,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(l2),new LabelList(l2,NULL)))),new ir_seq(new ir_label(l1),
                                                        new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e1,new ir_const(0),l3,novoRotulo2),new ir_seq(new ir_label(novoRotulo2),new ir_jump(new ir_name(l2),new LabelList(l2,NULL)))),new ir_seq(new ir_label(l3),new ir_seq(new ir_move(a->codigoAcesso(),new ir_const(1)),new ir_label(l2))))))),a->codigoAcesso());
        }else if (op == T_OR){
            exprGERAL = new ir_eseq(new ir_seq(new ir_move(a->codigoAcesso(),e1),new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e2,new ir_const(0),l1,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(l2),new LabelList(l2,NULL)))),new ir_seq(new ir_label(l1),new ir_seq(new ir_move(a->codigoAcesso(),new ir_const(1)),new ir_label(l2))))),a->codigoAcesso());
        }
    }
}
void Visitor_Translate_IR::visit(node_op_multiplication* node){
    //Mulit,Div, e Mod
    if(node!= NULL){
        int op = node->attr_operator;
        node->attr_expr_1->accept(this);
        ir_expr* e1 = exprGERAL;
        node->attr_expr_2->accept(this);
        ir_expr* e2 = exprGERAL;
        exprGERAL = new ir_binop(op,e1,e2);
    }
}
void Visitor_Translate_IR::visit(node_op_bitwise* node){
    int op = node->attr_operator;
    node->attr_expr_1->accept(this);
    ir_expr* e1 = exprGERAL;
    node->attr_expr_2->accept(this);
    ir_expr* e2 = exprGERAL;
    exprGERAL = new ir_binop(op,e1,e2);
}
void Visitor_Translate_IR::visit(node_op_relational* node){
    //Equal,NotEqual,LessThen,GreaterThen,LessEqualThen,GreaterEqualThen,
    if(node != NULL ){
        int oprel   = node->attr_operator;
        node->attr_expr_1->accept(this);
        ir_expr* e1 = exprGERAL;
        node->attr_expr_2->accept(this);
        ir_expr* e2 = exprGERAL;
        AcessoLocal* a = current_frame->adicionaLocal(true,4);
        label* l1 = new label();
        label* l2 = new label();
        label* novoRotulo = new label();


        exprGERAL = new ir_eseq(new ir_seq(new ir_move(a->codigoAcesso(), new ir_const(1)) ,new ir_seq(new ir_seq(new ir_cjump(oprel,e1,e2,l1,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(l2),new LabelList(l2,NULL)))),new ir_seq(new ir_label(l2),new ir_seq(new ir_move(a->codigoAcesso(),new ir_const(0)),new ir_label(l1)))) ),a->codigoAcesso());
    }
}
void Visitor_Translate_IR::visit(node_return* node){
    if(node->attr_expr) node->attr_expr->accept(this);
    stmtGERAL = new ir_move(new ir_temp(current_frame->attr_temp),exprGERAL);
}
void Visitor_Translate_IR::visit(node_stmt_list* node){
    if(node != NULL ){
        ir_stmt* seq1;
        ir_stmt* seq2;
        if(node->attr_stmt       != NULL) {
            node->attr_stmt->accept(this);
            seq1 = stmtGERAL;
        }
        if(node->attr_stmt_list  != NULL) {
            node->attr_stmt_list->accept(this);
            seq2 = stmtGERAL;
        }else{
            seq2 = NULL;
        }

        if((seq1 == NULL)&&(seq2 == NULL)){//os 2 são NULL
            stmtGERAL = NULL;
        }else if(seq1 != NULL){// primeiro nao NULL
            if(seq2 != NULL){ //os 2 diferentes
                stmtGERAL = new ir_seq(seq1,seq2);
            }else{ // primeiro tem e segundo NULL
                stmtGERAL = seq1;
            }
        }else{ //primeiro NULL e segundo nao
            stmtGERAL = seq2;
        }
    }
}
void Visitor_Translate_IR::visit(node_while* node){
    if(node != NULL ){
        label* teste  = new label();
        label* inicio = new label();
        label* fim    = new label();
        label* novoRotulo = new label();

        node->attr_expr->accept(this);
        ir_expr* e   = exprGERAL;
        node->attr_stmt->accept(this);
        ir_stmt* s   = stmtGERAL;

        stmtGERAL = new ir_seq(new ir_label(teste),new ir_seq(new ir_seq(new ir_cjump(T_NOT_EQUAL,e,new ir_const(0),inicio,novoRotulo),new ir_seq(new ir_label(novoRotulo),new ir_jump(new ir_name(fim),new LabelList(fim,NULL)))),new ir_seq(new ir_label(inicio),new ir_seq(s,new ir_seq(new ir_jump(new ir_name(teste),new LabelList(teste,NULL)),new ir_label(fim))))));
    }
}
void Visitor_Translate_IR::visit(node_negation* node){ //negação de true, false
    //COMO FAZER?
    if(node->attr_expr)node->attr_expr->accept(this);
    ir_expr* expr = exprGERAL;
   // exprGERAL = new ir_binop(T_PLUS,new ir_binop(T_NOR_UNI,expr,new ir_const(0)),ir_const(1));

}

void Visitor_Translate_IR::visit(node_signal* node){   //negação de numeros

    if(node->attr_expr) node->attr_expr->accept(this);
    ir_expr* expr = exprGERAL;
    exprGERAL = new ir_binop(T_PLUS,new ir_binop(T_NOR_UNI,expr,new ir_const(0)),new ir_const(1));
}

void Visitor_Translate_IR::visit(node_read* node){
    if(node->attr_expr) node->attr_expr->accept(this);
    stmtGERAL =  new ir_expr_void(new ir_call(new ir_name(new label("READ")),new ir_expr_list((exprGERAL),NULL)));
}

void Visitor_Translate_IR::visit(node_println* node){
    ir_expr_list* el = new ir_expr_list(NULL,NULL);
    ir_expr_list* el_aux = el;

    node_expr_list* e = node->attr_expr_list;
    while(e != NULL){
        e->attr_expr->accept(this);
        el_aux->attr_expr = exprGERAL;
        e = e->attr_expr_list;
        if(e != NULL) {
            el_aux->attr_expr_list = new ir_expr_list(NULL,NULL);
            el_aux = el_aux->attr_expr_list;
        }
    }

    stmtGERAL = new ir_expr_void(new ir_call(new ir_name(new label("PRINTLN")),el));

}

void Visitor_Translate_IR::visit(node_expr_list* node){
}

void Visitor_Translate_IR::visit(node_vector* node){
    ir_expr* local = NULL;
    Symbol_variable* id = ((Symbol_variable*)node->attr_name->attr_id);
    if(node->attr_expr){
        node->attr_expr->accept(this);
    }
    local = /*new ir_binop(T_MINUS,*/new ir_binop(T_MULT,new ir_binop(T_PLUS,exprGERAL,new ir_const(1)),new ir_const(FrameMIPS::wordSize))/*,new ir_const(FrameMIPS::wordSize))*/;

    if(!(id->is_attribute)) exprGERAL = new ir_mem(new ir_binop(T_PLUS,id->acesso->codigoAcesso(),local));
    else{
        if(exprNAME_LIST == NULL){ //Foi chamado sozinho sem this
            exprGERAL = new ir_mem(new ir_binop(T_PLUS,new ir_mem(new ir_binop(T_PLUS,new ir_const(id->offset),current_frame->attr_this->codigoAcesso())),local));
        }else{
            exprGERAL = new ir_mem(new ir_binop(T_PLUS,new ir_mem(new ir_binop(T_PLUS,new ir_const(id->offset),exprNAME_LIST->attr_exp2)),local));
            exprNAME_LIST = NULL;
        }
    }
}

void Visitor_Translate_IR::visit(node_new* node){}

void Visitor_Translate_IR::visit(node_length* node){


}
void Visitor_Translate_IR::visit(node_type* node){}



ir_expr* Visitor_Translate_IR::AlocaArray(int quant){

    AcessoLocal* acesso = (inFrame*)current_frame->adicionaLocal(true,quant*FrameMIPS::wordSize);
    return acesso->codigoAcesso();
}
ir_expr* Visitor_Translate_IR::AlocaObject(Symbol_class* classe){
    inFrame* acesso = (inFrame*)current_frame->adicionaLocal(true,classe->offset);
    return acesso->codigoEndereco();
}

ir_seq* Visitor_Translate_IR::initArray(ir_expr* base,int quant){
    ir_seq* seq = new ir_seq(NULL,NULL);
    seq->attr_stmt1 = new ir_move(new ir_mem(new ir_binop(T_PLUS,base,new ir_const(0))),new ir_const(quant));
    seq->attr_stmt2 = new ir_seq(NULL,NULL);
    ir_seq* seqaux = (ir_seq*)seq->attr_stmt2;


    for(int i=1; i<=quant; i++){
        ir_expr* end_Atrib = new ir_binop(T_PLUS,base,new ir_const(i*FrameMIPS::wordSize));

        seqaux->attr_stmt1 = new ir_move(new ir_mem(end_Atrib),new ir_temp(FrameMIPS::zero));
        seqaux->attr_stmt2 = new ir_seq(NULL,NULL);
        if(i!=quant)seqaux = (ir_seq*)seqaux->attr_stmt2;
    }

    seqaux->attr_stmt2 = NULL;
    return seq;
}

ir_seq* Visitor_Translate_IR::initObject(ir_expr* base, Symbol_class* classe){
    Symbol_variable** atributos = classe->array_variable;
    ir_seq* seq = new ir_seq(NULL,NULL);
    ir_seq* seqaux = seq;

    if(classe->father != NULL){
        seqaux->attr_stmt1 = initObject(base,classe->father);
        seqaux->attr_stmt2 = new ir_seq(NULL,NULL);current_frame->adicionaLocal(true,classe->offset);
        seqaux = (ir_seq*)seqaux->attr_stmt2;

    }

    //Construtor Padrão
    for(int i=0;i<classe->num_attributo;i++){
        ir_expr* end_Atrib = new ir_binop(T_PLUS,base,new ir_const(atributos[i]->offset));

        //objeto
        if(atributos[i]->type_object!=NULL){ //Se for um atributo tipo OBJETO
            if(atributos[i]->inicializador == NULL ){
                seqaux->attr_stmt1 = new ir_move(new ir_mem(end_Atrib),new ir_temp(FrameMIPS::zero));
            }
            else{
                Symbol_class* classe2 = atributos[i]->type_object;
                //Aloca espaço no Frame
                ir_expr_list* sl = new ir_expr_list(new ir_const(classe2->offset),NULL);
                ir_move* alocacao = new ir_move(new ir_mem(end_Atrib),new ir_call(new ir_name(new label("MALLOC")),sl));
                //INICIALIZAR O OBJETO;
                ir_seq* inicializacao = initObject(new ir_mem(end_Atrib),classe2);
                ir_seq* seq2 = new ir_seq(alocacao,inicializacao);//primeiro aloca, segundo construtor
                seqaux->attr_stmt1 = seq2;
            }
        }else if(atributos[i]->array==true){//vetor
            if(atributos[i]->inicializador == NULL ){
                seqaux->attr_stmt1 = new ir_move(new ir_mem(end_Atrib),new ir_temp(FrameMIPS::zero));
            }
            else{
                ir_move* alocacao = NULL;
                ir_expr* numVetor = atributos[i]->inicializador;
                //Se puder alocar em tempo de compilação, coloca no Frame
                if(typeid(*numVetor) == typeid(ir_const)){
                    int quant = ((ir_const*)numVetor)->attr_i;
                    ir_expr_list* el_vetor = new ir_expr_list(new ir_const(quant*FrameMIPS::wordSize+4),NULL);
                    alocacao = new ir_move(new ir_mem(end_Atrib),new ir_call(new ir_name(new label("MALLOC")),el_vetor));
                    //INICIALIZAR O OBJETO;
                    ir_seq* inicializacao = initArray(new ir_mem(end_Atrib),quant);
                    ir_seq* seq2 = new ir_seq(alocacao,inicializacao);//primeiro aloca, segundo construtor
                    seqaux->attr_stmt1 = seq2;
                }else{ //Senão, coloca na heap;



                }
            }
        }else{ //Se for um atributo tipo primitivo
            ir_expr* construtor = atributos[i]->inicializador;
            if(construtor==NULL) construtor = new ir_temp(FrameMIPS::zero);
            seqaux->attr_stmt1 = new ir_move(new ir_mem(end_Atrib),construtor);
        }
        seqaux->attr_stmt2 = new ir_seq(NULL,NULL);
        if(i!=classe->num_attributo-1)seqaux = (ir_seq*)seqaux->attr_stmt2;
    }
    seqaux->attr_stmt2 = NULL;
    return seq;
}

//Funcção para concatenação
char* itoa(int val, int base){
    static char buf[32] = {0};
    int i = 30;
    for(; val && i ; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}

int temp::attr_num  = 0;
int temp::attr_mod  = 1;
temp::temp(){
    attr_num++;
    if(attr_num % 10 == 0 ) attr_mod++;
    attr_string = (char*)malloc(sizeof(char)*(strlen("Temp$")+attr_mod));
    attr_string = strcpy(attr_string,"Temp$");
    attr_string = strcat(attr_string,itoa(attr_num,10));
}

temp::temp(char* string){
    attr_string = string;
}
char* temp::toString(){
    return attr_string;
}
temp::~temp(){}

int label::attr_num = 0;
int label::attr_mod = 1;
label::label(){
    attr_num++;
    if(attr_num % 10 == 0 ) attr_mod++;
    attr_string = (char*)malloc(sizeof(char)*(strlen("Label_")+attr_mod));
    attr_string = strcpy(attr_string,"Label_");
    attr_string = strcat(attr_string,itoa(attr_num,10));
    attr_id = attr_num;
}

label::label(char* string){
    attr_string = string;
    attr_num++;
    if(attr_num % 10 == 0 ) attr_mod++;
    attr_id = attr_num;
}
char* label::toString(){
    return attr_string;
}
label::~label(){}

LabelList::LabelList(label* l, LabelList* prox){
    attr_label = l;
    attr_next  = prox;
}
LabelList::~LabelList(){
    delete attr_label;
    delete attr_next;
}

inFrame::inFrame(int offset){
    attr_offset = offset;
    fp = new ir_temp(FrameMIPS::fp); //armazeno o fp do objeto;
}
ir_expr* inFrame::codigoAcesso(){
   // return new ir_binop(T_MINUS,fp,new ir_const(attr_offset));
    return new ir_mem(new ir_binop(T_MINUS,fp,new ir_const(attr_offset)));
}

ir_expr* inFrame::codigoEndereco(){
    return new ir_binop(T_MINUS,fp,new ir_const(attr_offset));
}
inFrame::~inFrame(){}

inReg::inReg(temp* temp){
    attr_temp = temp;
}
ir_expr* inReg::codigoAcesso(){
    return new ir_temp(attr_temp);
}
inReg::~inReg(){}

ListaAcesso::ListaAcesso(AcessoLocal* l, ListaAcesso* next){
    attr_l    = l;
    attr_next = next;
}
ListaAcesso::~ListaAcesso(){
    delete attr_l;
    delete attr_next;
}

int FrameMIPS::wordSize = 4;
//registradores do MIPS
temp* FrameMIPS::fp = new temp("fp");
temp* FrameMIPS::zero = new temp("zero");
FrameMIPS::FrameMIPS(label* l, temp* t, ListaAcesso* la,ListaAcesso* lp){
    attr_label       = l;
    attr_temp        = t;
    attr_ListaAcesso = la;
    attr_ListaAcessoParam = lp;
    attr_size        = 0 ;
    attr_this        = NULL;
}
FrameMIPS::~FrameMIPS(){
    delete attr_label;
    delete attr_temp;
    delete attr_ListaAcesso;
    delete attr_this;
}
AcessoLocal* FrameMIPS::adicionaParam(bool scape, int tamBytes){
    if(scape){
        inFrame* inF = new inFrame(attr_size);
        attr_size += tamBytes;
        return inF;
    }
    return new inReg(new temp());
}
AcessoLocal* FrameMIPS::adicionaLocal(bool scape, int tamBytes){
    if(scape){
        inFrame* inF = new inFrame(attr_size);
        attr_size += tamBytes;
        return inF;
    }
    return new inReg(new temp());
}

Literal::Literal(char* string){
    attr_string = string;
    attr_next   = NULL;
    attr_label  = new label(attr_string);
}
Literal::~Literal(){
    delete attr_string;
    delete attr_next;
}
void Literal::accept(Visitor_fragment* visitor){
    if (visitor != NULL) visitor->visit(this);
}

Procedure::Procedure(Frame* frame, ir_stmt* body){
    attr_frame = frame;
    attr_body   = (body !=NULL )?new ir_stmt_list(body,NULL):NULL;
    attr_next   = NULL;
}
Procedure::~Procedure(){
    delete attr_frame;
    delete attr_body;
    delete attr_next;
}
void Procedure::accept(Visitor_fragment* visitor){
    if (visitor != NULL) visitor->visit(this);
}


Fragment* Visitor_Translate_IR::getFirstFragment(){
    if (this->first_fragment != NULL){
        return this->first_fragment;
    }
    return NULL;
}

Fragment* Visitor_Translate_IR::getCurrentFragment(){
    if (this->current_fragment != NULL){
        return this->current_fragment;
    }
    return NULL;
}
