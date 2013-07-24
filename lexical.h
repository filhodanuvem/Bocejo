#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include <stdio.h> // incluido por causa da estrutura FILE*
#include "symbols_tables.h" // incluido por causa dos metodos que retornam tabelas de simbolos

#define BUFFER_LENGTH 3
#define ARRAY_LENGTH 20
#define ARRAY_MORE_SPACE 50

// tokens
#define T_EOF 103
#define T_CLASS 104
#define T_ID 105
#define T_EXTENDS 106
#define T_PUBLIC 107
#define T_STATIC 108
#define T_VOID 109
#define T_INT 110
#define T_FLOAT 111
#define T_BOOLEAN 112
#define T_IF 113
#define T_PARENT_L 114
#define T_PARENT_R 115
#define T_WHILE 116
#define T_SYSTEM 117
#define T_OUT 118
#define T_IN 119
#define T_PRINTLN 120
#define T_READ 121
#define T_RETURN 122
#define T_BLOCK_BEGIN 123
#define T_BLOCK_END 124
#define T_NEW 125
#define T_NUM_REAL 126
#define T_NUM_INT 127
#define T_LITERAL 128
#define T_LENGTH 129
#define T_BRACKET_L 130
#define T_BRACKET_R 131
#define T_TRUE 132
#define T_FALSE 133
#define T_THIS 134
#define T_MINUS 135
#define T_NEG 136
#define T_PLUS 137
#define T_EQUAL 138
#define T_LESS_T 139
#define T_LESS_E 140
#define T_GREATER_T 141
#define T_GREATER_E 142
#define T_NOT_EQUAL 143
#define T_OR_UNI 144
#define T_NOR_UNI 158
#define T_MULT 145
#define T_DIV 146
#define T_MOD 147
#define T_AND_UNI 148
#define T_AND 149
#define T_OR 150
#define T_DOT 151
#define T_COMMA 152
#define T_SEMICOLON 153
#define T_ASSIGN 154
#define T_ELSE 155
#define T_PROTECTED 156
#define T_PRIVATE 157


void lex_constructor(FILE* file);
void lex_destructor();
void lex_load_buffer();
char lex_next_char();
int lex_next_token();
int GET_current_line();

const char* GET_token_name(int token);
void SET_array(char caracter);
char* GET_current_lexeme();
Symbols_table_rw* GET_symbol_table_rw();
Symbols_table_id* GET_symbol_table_id();
Symbols_table_literal* GET_symbol_table_lit();
#endif // LEX_H_INCLUDED
