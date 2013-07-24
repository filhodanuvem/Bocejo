/**
* semantic.cpp
* @author Claudson Oliveira
* @date   20 de maio de 2011
*
* Arquivo que implementa tudo do módulo semântico
*
**/

#include "semantic.h"
#include "lexical.h"
#include "visitors.h"
#include "error.h"
#include "abstract_syntax_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include "string.h"

static scope* current_scope = NULL; // pilha de escopos do semântico
static int current_level = -1;
static scope* current_this = NULL; // para automatizar o acesso usando this, ja temos essa variavel que sempre aponta para o 'fundo' da pilha.
static Symbols_table_id* table_id = GET_symbol_table_id();
static Symbols_table_id* table_class = GET_symbol_table_id();

scope::scope(){
    this->table_variable = new Symbols_table_id();
    this->table_method   = new Symbols_table_id();
    this->next = NULL;
}

scope::~scope(){
    delete this->table_method;
    delete this->table_variable;
    this->next = NULL;
}

void scope::put(Symbol_variable* symbol, char* key){
    this->table_variable->insert(key,symbol);
}

void scope::put(Symbol_method* symbol, char* key){
    this->table_method->insert(key,symbol);
}

void put(Symbol_class* symbol, char* key){
    current_scope->current_class = symbol;
  //  symbol->current_scope = current_this;
    table_class->insert(key,symbol);
}

Symbol_class* GET_class(char* key){
    return (Symbol_class*) table_class->search(key);
}

Symbol_variable* scope::get_variable(char* key){
    scope* p = this;
    Symbol_variable* var = NULL;
    // procura variavel dentro da classe
    do{
        var = (Symbol_variable*)p->table_variable->search(key);
        if(var != NULL) return var;
        if(p->next != NULL) p = p->next;
        else break;
    }while(p != NULL);

    while(p != NULL){
       var = (Symbol_variable*) p->table_variable->search(key);
       if(var != NULL ) return var;
       if(p->current_class->father != NULL ) p = p->current_class->father->current_scope;
       else break;
    }
    return NULL;
}

Symbol_method* scope::get_method(char* key){

    scope* p = this; // só existem métodos no escopo zero
    Symbol_method* meth = NULL;
    while(p->next != NULL) p = p->next; // vai ate o escopo da classe associada
    while(p != NULL){
        meth = (Symbol_method*)p->table_method->search(key);
        if(meth != NULL) return meth;
        if(p->current_class->father) p = p->current_class->father->current_scope;
        else p = NULL;
    }
    return NULL;
}

void semantic_constructor(){
   if(!is_ok()) semantic_error(E_FATAL_ERROR,0); // não há análise semântica se houver algum erro sintático
   table_class = new Symbols_table_id();
}

void semantic_destructor(){
    table_id->destroy_symbols();
    table_class->destroy_symbols();
    delete table_class;
}

/*
* Cria e empilha um escopo na pilha de escopos,
* se estivermos no nevel zero (nivel de classes) setamos
* um novo escopo this para facilitar acessos correntes
*/
void begin_scope(){
    current_level++;
    scope* new_scope = new scope();
    new_scope->next = current_scope;
    current_scope = new_scope;
    if(current_level == 0 ) current_this = current_scope;

}

/*
* Desempilha um escopo da pilha e o destroi se este escopo não for
* pertencente ao nível de classe, caso contrário ele fica na memória pois
* está sendo usado pela tabela de simbolos de classes, por exemplo
*/
void end_scope(){
    current_level--;
    scope* dead_scope = current_scope;
    current_scope = current_scope->next;
    if(current_level > -1 ) delete dead_scope ;
}

scope* GET_current_scope(){
    return current_scope;
}

scope* GET_scope_this(){
    return current_this;
}

Symbols_table_id* GET_table_class(){
    return table_class;
}

int GET_scope_level(){
    return current_level;
}

/*
* Testa se a variavel de nome variable existe no escopo mais local possível
* e vai subindo na pilha, caso não seja nem um atributo do objeto corrente
* chama a função que trata esse tipo de busca para hierarquias
*/

bool is_declared(char* variable,scope* start){
    scope* p = start;
    bool error = true;
    do{
        if(p->get_variable(variable) != NULL) return true; // achei variavel dentro da minha classe
        else if(p->next != NULL) p = p->next;
             else break;
    }while(p->next != NULL);
    return is_declared_inherited(variable,p); // retornar se encontrou variavel em ancestral
}

/*
*  testa se a variavel de string variable foi declarado no escpo start ou em qualquer um dos
*  seus pais (busca de atributos de objeto corrente ou em seus ancestrais)
*/

bool is_declared_inherited(char* variable,scope* start){
    scope* p = start; // a busca se inicia do this para seus ancestrais
    int error =  1;
    if(p->current_class != NULL){
        do{
            if(p->get_variable(variable)!= NULL ) return true;
             if(p->current_class->father!= NULL ) p = p->current_class->father->current_scope; // pega o scopo do pai
             else break;
        }while(p  != NULL);
    }
    return false;
}

/*
*  Função que transforma um nó de declaração de variável em um símbolo específico
*/

Symbol_variable* create_variable(node_name_decl* node){
    Symbol_variable* var = new Symbol_variable();
    var->type_primitive  = node->attr_type->attr_primitive;
    if(var->type_primitive > T_EOF && node->attr_type->attr_array) var->array = 1;
    else var->array = 0;
    var->type_object = NULL;
    return var;
}
/*
*  função que transforma o nó da asa de declaração de método num símbolo específico
*  a lógia de parametros também está inserida
*/
Symbol_method* create_method(node_method_list* node){
    Symbol_method* method = new Symbol_method();
    method->return_primitive = node->attr_type->attr_primitive;
    method->return_object = NULL;
    if(node->attr_type->attr_id) method->return_object    = (Symbol_class*)node->attr_type->attr_id;
    method->array            = node->attr_type->attr_array;
    method->overload         = NULL;
    method->access           = node->attr_access;
    method->owner            = current_this->current_class;
    node_var_list* var = node->attr_param_list;
    int param_numbers = 0;
    while(var != NULL){
        param_numbers++;
        if(var->attr_var_list != NULL) var = var->attr_var_list;
        else var = NULL;
    }

    method->params = (Symbol_variable**) malloc(sizeof(Symbol_variable*)*param_numbers);
    var = node->attr_param_list;
    method->num_params = param_numbers;
    param_numbers = 0;
    while(var != NULL){
        Symbol_variable* param = new Symbol_variable();
        param->type_object    = NULL;
        param->type_primitive = var->attr_name->attr_type->attr_primitive;
        param->array          = var->attr_name->attr_type->attr_array;
        method->params[param_numbers++] = param;
        if(var->attr_var_list != NULL) var = var->attr_var_list;
        else var = NULL;
    }
    return method;
}
/*
* Função que testa se meth tem ou não uma nova assinatura se comparado ao método saved
*/

bool is_new_signature(Symbol_method* meth,Symbol_method* saved){

    if(meth->num_params != saved->num_params ) return true;
    Symbol_variable** param_1 = meth->params;
    Symbol_variable** param_2 = saved->params;

    for(int i = 0; i < meth->num_params; i++){
        if(param_1[i]->type_primitive != param_2[i]->type_primitive) return true;
        else if(param_1[i]->type_object != param_2[i]->type_object) return true;
    }
    return false;
}
