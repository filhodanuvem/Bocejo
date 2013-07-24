/**
* lexical.c
* @author Bruno Guimarães
* @date   08 de abril de 2011
*
* Arquivo que implementa tudo do módulo do analisador lexico.
*
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexical.h"
#include "error.h"
#include "symbols_tables.h"

static FILE * file = NULL;
static char buffer[BUFFER_LENGTH];

static int  next_position_buffer = 0  ;
static char current_char;

static char * array_lexeme = NULL;
static int current_position_array_lexeme; //Arranjo dinamico //PD

static unsigned int current_line = 1; //Contador de Linha
static Symbols_table_id      * table_id      = new Symbols_table_id();
static Symbols_table_rw      * table_rw      = new Symbols_table_rw();
static Symbols_table_literal * table_literal = new Symbols_table_literal();

static unsigned int length_array_lexeme = 0 ;

 static char* tokens_name[] = {"T_EOF", "T_CLASS", "T_ID", "T_EXTENDS", "T_PUBLIC", "T_STATIC", "T_VOID", "T_INT","T_FLOAT","T_BOOLEAN","T_IF",
                        "T_PARENT_L","T_PARENT_R","T_WHILE","T_SYSTEM","T_OUT","T_IN","T_PRINTLN","T_READ","T_RETURN","T_BLOCK_BEGIN","T_BLOCK_END",
                        "T_NEW","T_NUM_REAL","T_NUM_INT","T_LITERAL","T_LENGTH","T_BRACKET_L","T_BRACKET_R","T_TRUE","T_FALSE","T_THIS","T_MINUS",
                        "T_NEG","T_PLUS","T_EQUAL","T_LESS_T","T_LESS_E","T_GREATER_T","T_GREATER_E","T_NOT_EQUAL","T_OR_UNI", "T_MULT","T_DIV","T_MOD",
                        "T_AND_URI","T_AND","T_OR","T_DOT","T_COMMA","T_SEMICOLON","T_ASSIGN","T_ELSE","T_PROTECTED","T_PRIVATE"
                        };
/*
* Construtor do analisador léxico
*/
void lex_constructor(FILE* p_file){
    file = p_file;
    length_array_lexeme  = ARRAY_LENGTH + 1;
    array_lexeme = (char*)malloc(length_array_lexeme * sizeof(char));
    array_lexeme[length_array_lexeme - 1] = '\0';

    lex_load_buffer(); //Preencher o Buffer pela primeira vez
    current_char = lex_next_char(); //Ja começa pegando o primeiro char
}

/*
* Metodo que desaloca todas as memorias e fecha o arquivo
*/
void lex_destructor(){

//  table_rw->show();
//  table_id->show();
//   table_literal->show();
    delete table_id;
    delete table_literal;
    delete table_rw;
    free(array_lexeme);
    fclose(file);
}


/*
* Metodo que carrega até BUFFER_LENGTH caracteres pro buffer.
*/
void lex_load_buffer(){
    int bytes_reads = BUFFER_LENGTH;
    int reads       = fread(buffer,sizeof(char),bytes_reads,file); // retorna numero de linhas lidas

    if(feof(file)) buffer[reads] = EOF;
    else buffer[reads]           = 0; // fim do buffer (sentinela)
    next_position_buffer         = 0;
}

/*
* Função que retorna o proximo caracter
*/
char lex_next_char(){

    char next_char = buffer[next_position_buffer];
    if (next_char == '\n') current_line++; //Contador de Linha usado no gerenciador de erros

    next_position_buffer++;
    // caso encontre o sentinela -fim de buffer- recarregue-o
    if(buffer[next_position_buffer] == 0 ) lex_load_buffer();
    return next_char;
}


int lex_next_token(){
    current_position_array_lexeme = 0;
    int state              = 0;
    Symbol_rw * symbol     = NULL;
    while(1){
         switch(state){
            case 0: //Estado inicial do automato
                if(isalpha(current_char)){
                    state = 1;
                    SET_array(current_char);
                }else if(isdigit(current_char)){
                    state = 5;
                    SET_array(current_char);
                }else if(isspace(current_char)){
                    state = 0;
                }else{
                    switch(current_char){
                        case '"':
                            state = 3;
                            SET_array(current_char); //Deve armazenar o " inicial
                            break;
                        case '.': state = 7;  break;
                        case '(': state = 20; break;
                        case ')': state = 21; break;
                        case '{': state = 22; break;
                        case '}': state = 23; break;
                        case ']': state = 24; break;
                        case '[': state = 25; break;
                        case '-': state = 26; break;
                        case '+': state = 27; break;
                        case '*': state = 28; break;
                        case '!': state = 29; break;
                        case '=': state = 30; break;
                        case '|': state = 31; break;
                        case '&': state = 32; break;
                        case '%': state = 41; break;
                        case ',': state = 48; break;
                        case ';': state = 49; break;
                        case '>': state = 42; break;
                        case '<': state = 43; break;
                        case '/': state = 14; break;
                        case '_': state = 54; break;
                        case EOF: state = 19; break;
                        default:  state = 50;
                    }
                }
                current_char = lex_next_char();
                break;
            case 1:
                if(isalnum(current_char)){
                    state = 1;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else if(current_char=='_'){
                    state = 54;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    SET_array('\0');
                    state = 2;
                }
                break;
            case 2:
                    symbol = (Symbol_rw*)table_rw->search(array_lexeme);
                    if(symbol != NULL){ //Foi encontrado na Tabela de Palavra Reservada
                        int token = symbol->GET_token();
                        return token;
                    }
                    else{
                        table_id->insert(array_lexeme,new Symbol());
                        return T_ID;
                    }
                break;
            case 3: //Literal
                if(current_char == '"'){
                    state = 4;
                    SET_array(current_char);//armazena o " final
                    SET_array('\0');
                    current_char = lex_next_char(); //Caso diferente, a gente consume e chama o proximo
                }else if(current_char == EOF){
                    SET_array('\0');
                    state = 12;
                }else{
                    state = 3;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }
                break;
            case 4:
                table_literal->insert(array_lexeme,new Symbol());
                return T_LITERAL;
                break;
            case 5:
                if(isdigit(current_char)){
                    state = 5;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else if(current_char == '.'){
                    state = 10;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else if((current_char == 'E')||(current_char == 'e')){
                    state = 9;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    state = 6;
                    SET_array('\0');
                }
                break;
            case 6: return T_NUM_INT; break;
            case 7:
                if(isdigit(current_char)){
                    state = 10;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else state = 8;
                break;
            case 8: return T_DOT; break;
            case 9:
                if(isdigit(current_char)){
                    state = 51;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else if((current_char == '+')||(current_char == '-')){
                    state = 11;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    state = 13;
                    SET_array('\0');
                }
                break;
            case 10:
                if(isdigit(current_char)){
                    state = 10;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else if((current_char == 'E')||(current_char == 'e')){
                    state = 9;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    state = 13;
                    SET_array('\0');
                }
                break;
            case 11:
                if(isdigit(current_char)){
                    state = 51;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else state = 52;
                break;
            case 12:
                lexical_error(E_EOF_IN_LITERAL,current_line);
                return T_EOF; //Apesar de Erro retorna o EOF pois é o encontrado
                break;
            case 13: return T_NUM_REAL; break;
            case 14:
                if(current_char == '/'){ //Comentario de uma linha
                    state = 18;
                    current_char = lex_next_char();
                }else if (current_char == '*'){
                    state = 15;
                    current_char = lex_next_char();
                }else state = 16;
                break;
            case 15:
                if(current_char == EOF){
                    state = 53;
                }else if (current_char == '*'){
                    state = 17;
                    current_char = lex_next_char();
                }else{
                    state = 15;
                    current_char = lex_next_char();
                }
                break;
            case 16: return T_DIV; break;
            case 17:
                if (current_char == '*'){
                    state = 17;
                    current_char = lex_next_char();
                }else if(current_char == '/'){
                    state = 0;
                    current_char = lex_next_char();
                }else {
                    state = 15;
                    current_char = lex_next_char();
                }
                break;
            case 18:
                if(current_char == '\n'){
                    state = 0;
                    current_char = lex_next_char();
                }else{
                    if(current_char == EOF){ // tratando EOF no fim do comentário de uma linha
                        lexical_error(E_EOF_IN_COMMENT,current_line);
                        return T_EOF;
                    }
                    state = 18;
                    current_char = lex_next_char();
                }
                break;
            case 19: fflush(file); return T_EOF; break;
            case 20: return T_PARENT_L; break;
            case 21: return T_PARENT_R; break;
            case 22: return T_BLOCK_BEGIN; break;
            case 23: return T_BLOCK_END; break;
            case 24: return T_BRACKET_R; break;
            case 25: return T_BRACKET_L; break;
            case 26: return T_MINUS; break;
            case 27: return T_PLUS; break;
            case 28: return T_MULT; break;
            case 29:
                if (current_char == '='){
                    state = 33;
                    current_char = lex_next_char();
                }else{
                    state = 34;
                }
                break;
            case 30:
                if (current_char == '='){
                    state = 35;
                    current_char = lex_next_char();
                }else state = 36;
                break;
            case 31:
                if (current_char == '|'){
                    state = 37;
                    current_char = lex_next_char();
                }else state = 38;
                break;
            case 32:
                if (current_char == '&'){
                    state = 39;
                    current_char = lex_next_char();
                }else state = 40;
                break;
            case 33: return T_NOT_EQUAL; break;
            case 34: return T_NEG; break;
            case 35: return T_EQUAL; break;
            case 36: return T_ASSIGN; break;
            case 37: return T_OR; break;
            case 38: return T_OR_UNI; break;
            case 39: return T_AND; break;
            case 40: return T_AND_UNI; break;
            case 41: return T_MOD; break;
            case 42:
                if (current_char == '='){
                    state = 44;
                    current_char = lex_next_char();
                }else state = 45;
                break;
            case 43:
                if (current_char == '='){
                    state = 46;
                    current_char = lex_next_char();
                }else state = 47;
                break;
            case 44: return T_GREATER_E; break;
            case 45: return T_GREATER_T; break;
            case 46: return T_LESS_E; break;
            case 47: return T_LESS_T; break;
            case 48: return T_COMMA; break;
            case 49: return T_SEMICOLON; break;
            case 50:
                lexical_error(E_INVALID_CHAR,current_line);
                current_position_array_lexeme = 0; //Zera o array
                state = 0;//voltar para estado inicial não retornando nada
                break;
            case 51:
                if(isdigit(current_char)){
                    state = 51;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    state = 13;
                    SET_array('\0');
                }
                break;
            case 52:
                lexical_error(E_FLOAT_END_SIGNAL,current_line);
                SET_array('\0');//Caso queria mostrar o erro
                current_position_array_lexeme = 0; //Zera o array
                state = 0;
                break;
            case 53:
                lexical_error(E_EOF_IN_COMMENT,current_line);
                return T_EOF; //Apesar de Erro retorna o EOF pois é o encontrado
                break;
            case 54:
                if(isalnum(current_char)||current_char=='_'){
                    state = 54;
                    SET_array(current_char);
                    current_char = lex_next_char();
                }else{
                    state = 55;
                    SET_array('\0');
                }
                break;
            case 55:
                table_id->insert(array_lexeme,new Symbol());
                return T_ID;
                break;
        }
    }
}
/*
*  Metodo que retorna o nome do token quando passado seu valor, o padrao é uma string vazia
*/
const char* GET_token_name(int token)
{
    return tokens_name[token - T_EOF];
}


/*
* Metodos SETs que possuem alguma logica extra a uma atribuição
* e metodos GETs para dar acesso a variaveis aos modulos externos
*/
void SET_array(char caracter){

    unsigned int empties_locals = length_array_lexeme - current_position_array_lexeme;

    if(empties_locals == 1){

      length_array_lexeme = length_array_lexeme + ARRAY_MORE_SPACE;
      char * cp = array_lexeme;
      array_lexeme = (char*)malloc(length_array_lexeme *sizeof(char));
      memcpy(array_lexeme,cp,strlen(cp));
      array_lexeme[length_array_lexeme - 1] = '\0';
      free(cp);
    }
    array_lexeme[current_position_array_lexeme] = caracter;
    current_position_array_lexeme++;
}

char* GET_current_lexeme(){

    return array_lexeme;
}

int GET_current_line(){

    return current_line;
}

// havia removido isto, mas é preciso prover uma interface entre as tabelas de simbolos e o parser
Symbols_table_id* GET_symbol_table_id(){

    return table_id;
}

Symbols_table_literal* GET_symbol_table_lit(){

    return table_literal;
}

Symbols_table_rw* GET_symbol_table_rw(){
    return table_rw;
}



