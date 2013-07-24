/**
* symbols_tables.cpp
* @author Claudson Oliveira
* @date   10 de abril de 2011
*
* Arquivo que implementa tudo do módulo de tabela de símbolos.
*
**/

#include "symbols_tables.h"
#include "lexical.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*
* Classe Simbolo
*/
Symbol::Symbol(){
    this->position = 0;
}

Symbol::Symbol(int pos){
    this->position = pos;
}

Symbol::~Symbol(){}

/*
*  Classe especiliazada de simbolo usada somente na tabela de palavras reservadas (Symbols_table_rw)
*/
Symbol_rw::Symbol_rw(int token){
    SET_token(token);
    this->position = 0;
}

void Symbol_rw::SET_token(int token_value){
    token = token_value ;
}

int Symbol_rw::GET_token()
{
    return token ; //simbolo normal não retorna token
}

Symbol_variable::Symbol_variable(){
    this->position = 0 ;
    offset         = 0;
    acesso         = NULL;
    inicializador  = NULL;
    is_attribute   = 0;
}

Symbol_class::Symbol_class(){
    array_variable = NULL;
}
/*
*  Classe Abstrata Tabelas de Simbolos
*/
Symbols_table::Symbols_table(){

    current_position_array_lexemes     = 0 ;
    length_array_lexemes = ARRAY_LEXEMES_LENGTH;
    table         = (entry*)malloc(PRIME_TABLE_LENGTH * sizeof(entry));
    array_lexemes = (char*)malloc(ARRAY_LEXEMES_LENGTH * sizeof(char));

    for(int i=0; i < PRIME_TABLE_LENGTH; i++) table[i] = NULL; // evitando lixo de memória
}

Symbols_table::~Symbols_table(){

    // libera totalmente a memória alocada pela tabela de símbolos
    // percorrendo cada nó das listas existentes e os liberando
    entry p_current = NULL, p_next = NULL;
    for(int i = 0; i < PRIME_TABLE_LENGTH ; i++ ){

        if(table[i] != NULL){

            p_next = table[i];
            while(p_next != NULL){
                p_current = p_next;
                p_next = p_next->next;
                p_current->info = NULL; // observação, não estamos mais destruindo simbolos
                free(p_current);
                p_current = NULL;
            }
        }
    }
    // por fim, libero o array table e outras estruturas alocadas
    free(table);
    free(array_lexemes);
    table   = NULL;
    array_lexemes = NULL;
}

/*
* Destroi simbolos associados a tabela
*/

void Symbols_table::destroy_symbols(){
    if(this == NULL) return;
    entry p_current = NULL, p_next = NULL;
    for(int i = 0; i < PRIME_TABLE_LENGTH ; i++ ){
        if(table[i] != NULL){
            p_next = table[i];
            while(p_next != NULL){
                p_current = p_next;
                p_next = p_next->next;
                delete p_current->info;
            }
        }
    }
}

/*
* função de Hash
*/
int Symbols_table::hash(char* lexeme){

    char* pointer;
    unsigned int h = 0, g=0;
    int key;

    for(pointer = lexeme; *pointer != 0; pointer++){
        h = (h << 4) + (*pointer);
        if(g == (h & 0xf0000000) ){
            h = pow(h,(g >> 24));
            h = pow(h,g);
        }
    }
    key  = h % PRIME_TABLE_LENGTH ;
    return key;
}

/*
* Método genérico para inserção numa tabela de símbolo
*/
int Symbols_table::insert(char* lexeme, Symbol* symbol){

    if(symbol == NULL) return 0;
    int key         = hash(lexeme);
    entry pointer   = table[key];
    entry new_entry = NULL;

    // Só insere na tabela caso seja um lexema ainda não inserido
    if(search(lexeme) == NULL){

        new_entry = (entry) malloc(sizeof(struct_node));
        // marca a posição do lexema adicionado no arranjao de lexemas
        new_entry->next           = NULL;
        new_entry->info           = symbol;
        new_entry->info->position = current_position_array_lexemes; //obriga a todos os simbolos que estao na tabela a apontarem para a posicao do arranjao
        SET_array_lexemes(lexeme);
        if(table[key] == NULL)   table[key] = new_entry;
        else{
           //Pra nao ter que percorrer, a inserção é O(1)
            new_entry->next = pointer->next;
            pointer ->next = new_entry;
        }
        return 1;
    }
}

/*
* Método que retorna o símbolo associado ao lexema usado para buscar
*/
Symbol* Symbols_table::search(char* lexeme)
{
    int key      = hash(lexeme);
    entry pointer = table[key];

    while(pointer != NULL){

        int pos = pointer->info->position;
        if(strcmp((const char*)&array_lexemes[pos] , (const char*) lexeme ) == 0) return pointer->info; // lexema encontrado na tabela!

        pointer = (entry)pointer->next;
    }
    return NULL;
}

char* Symbols_table::get_lexeme(Symbol* symbol){

   if(symbol == NULL) return NULL;
   int i = symbol->position;
   //while(i > 0 && array_lexemes[i-1] != '\0') i--;
   if(symbol->position < current_position_array_lexemes) return &array_lexemes[i];
   return NULL;
}

/*
* Metodo que imprime um símbolo
*/
void Symbols_table::print_symbol(Symbol* symbol){
    if(symbol->position <= current_position_array_lexemes) fprintf(stdout,"->%s",&array_lexemes[symbol->position]);
}

/*
*
*/
void Symbols_table::SET_array_lexemes(char* lexeme)
{
    int empties_locales = length_array_lexemes - current_position_array_lexemes ;
    int lexeme_length   = strlen(lexeme) + 1; // tamanho da string contando o \0

    // Caso preciso, realoca arranjao de lexemas para caber o novo lexema e sobrar ARRAY_LEXEMES_MORE_SPACE espaços
    // Obs: não nos preocupamos com o lixo de memória, já que nunca iremos acessar posições posterior
    // ao último \0

    if(lexeme_length > empties_locales){

      length_array_lexemes +=  lexeme_length + ARRAY_LEXEMES_MORE_SPACE;
      array_lexemes = (char*)realloc(array_lexemes,length_array_lexemes*sizeof(char));
    }
    // copia lexema para o arranjao de lexemas e movimenta o position
    // para a posição adequada
    strcpy((char*)&array_lexemes[current_position_array_lexemes],(const char*)lexeme);
    current_position_array_lexemes += lexeme_length;
}

Symbol** Symbols_table::getAtributos(){
    Symbol** symbols = NULL;
    int countSymbols = 0;
    int num          = 0;

    entry p_current = NULL, p_next = NULL;
    for(int i = 0; i < PRIME_TABLE_LENGTH ; i++ ){
        if(table[i] != NULL){
            p_next = table[i];
            while(p_next != NULL){
                countSymbols++;
                p_next = p_next->next;
            }
        }
    }
    symbols = (Symbol**) malloc((sizeof(Symbol*))*countSymbols);

    for(int i = 0; i < PRIME_TABLE_LENGTH ; i++ ){
        if(table[i] != NULL){
            p_next = table[i];
            while(p_next != NULL){
                symbols[num++] = p_next->info;
                p_next = p_next->next;
            }
        }
    }
    return symbols;
}

/*
*  A seguir os metodos do tipo to_string para cada tabela de simbolo
*/
void Symbols_table_rw::show()
{
    entry  pointer;
    fprintf(stdout,"\nTABELA DE SIMBOLOS : PALAVRAS RESERVADAS \n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    fprintf(stdout,"LEXEMA");
    fprintf(stdout,"\t \t \t Token numerico\n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    for(int i=0; i < PRIME_TABLE_LENGTH; i++){

        pointer = table[i];
        while(pointer != NULL){

            fprintf(stdout,"%s \t \t \t ",&array_lexemes[pointer->info->position]);
            int token = ((Symbol_rw*)pointer->info)->GET_token();
            if(token != 0) fprintf(stdout,"%d ",token);
            fprintf(stdout,"\n");
            pointer = pointer->next;
        }
    }
    fprintf(stdout,"\n");
}

void Symbols_table_literal::show()
{
    entry  pointer;
    int i;
    fprintf(stdout,"\nTABELA DE SIMBOLOS : LITERAIS \n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    fprintf(stdout,"LEXEMA \n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    for(i=0; i < PRIME_TABLE_LENGTH; i++){

        pointer = table[i];
        while(pointer != NULL){

            fprintf(stdout,"%s \t \t \t \n",&array_lexemes[pointer->info->position]);
            pointer = pointer->next;
        }
    }
    fprintf(stdout,"\n");

}

void Symbols_table_id::show(){

    entry  pointer;
    int i;
    fprintf(stdout,"\nTABELA DE SIMBOLOS : IDENTIFICADORES \n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    fprintf(stdout,"LEXEMA \n");
    fprintf(stdout,"-----------------------------------------------------------\n");
    for(i=0; i < PRIME_TABLE_LENGTH; i++){

        pointer = table[i];
        while(pointer != NULL){

            fprintf(stdout,"%s \t \t \t \n",&array_lexemes[pointer->info->position]);
            pointer = pointer->next;
        }
    }
    fprintf(stdout,"Array lexemes -> ");
    for(i=0;i < current_position_array_lexemes; i++ ) if(&array_lexemes[i] != 0) fprintf(stdout,"%c",array_lexemes[i]);
    else fprintf(stdout,"0");
    fprintf(stdout,"\n");
}

/*
* Construtor da tabela de simbolos de palavras reservadas, possui a particularidade
* de ja inserir todos os simbolos
*/
Symbols_table_rw::Symbols_table_rw(){

    insert("class"  ,new Symbol_rw(T_CLASS));
    insert("extends",new Symbol_rw(T_EXTENDS));
    insert("public" ,new Symbol_rw(T_PUBLIC));
    insert("protected" ,new Symbol_rw(T_PROTECTED));
    insert("private" ,new Symbol_rw(T_PRIVATE));
    insert("boolean",new Symbol_rw(T_BOOLEAN));
    insert("false"  ,new Symbol_rw(T_FALSE));
    insert("true"   ,new Symbol_rw(T_TRUE));
    insert("int"    ,new Symbol_rw(T_INT));
    insert("float"  ,new Symbol_rw(T_FLOAT));
    insert("if"     ,new Symbol_rw(T_IF));
    insert("else"   ,new Symbol_rw(T_ELSE));
    insert("length" ,new Symbol_rw(T_LENGTH));
    insert("while"  ,new Symbol_rw(T_WHILE));
    insert("new"    ,new Symbol_rw(T_NEW));
    insert("System" ,new Symbol_rw(T_SYSTEM));
    insert("println",new Symbol_rw(T_PRINTLN));
    insert("read"   ,new Symbol_rw(T_READ));
    insert("out"    ,new Symbol_rw(T_OUT));
    insert("in"     ,new Symbol_rw(T_IN));
    insert("return" ,new Symbol_rw(T_RETURN));
    insert("static" ,new Symbol_rw(T_STATIC));
    insert("this"   ,new Symbol_rw(T_THIS));
    insert("void"   ,new Symbol_rw(T_VOID));
}


