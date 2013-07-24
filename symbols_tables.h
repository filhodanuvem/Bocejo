#ifndef SYMBOLS_TABLES_H
#define SYMBOLS_TABLES_H

class scope;
class AcessoLocal;
class ir_expr;
class Symbol_variable;

#define PRIME_TABLE_LENGTH 37
#define ARRAY_LEXEMES_LENGTH 10
#define ARRAY_LEXEMES_MORE_SPACE 10

/*
*  Classe abstrata que representa o símbolo
*  e seus filhos concretos.
*  O simbolo é o elemento mais importante da tabela de simbolos,
* no caso da tabela de palavras reservada, o simbolo guarda o numero do token
*/


class Symbol {
    public:
        Symbol(int pos);
        Symbol();
        ~Symbol();
        int position; // posição no arranjao
};

class Symbol_rw: public Symbol{
    public:
        Symbol_rw(int token);
        void SET_token(int token_value);
       int GET_token();
    protected:
        int token;
};

class Symbol_id: public Symbol{};
class Symbol_class: public Symbol{
    public:
        Symbol_class();
        Symbol_class* father;
        scope* current_scope;
        int offset;
        int num_attributo;
        Symbol_variable** array_variable;
};

class Symbol_variable: public Symbol{
    public:
        Symbol_variable();
        Symbol_class* type_object;

        int type_primitive;
        int array;
        int offset;
        int is_attribute;
        AcessoLocal* acesso;
        ir_expr* inicializador;
};

class Symbol_method: public Symbol{
    public:
        Symbol_class* return_object;
        Symbol_class* owner ;
        int return_primitive;
        int array;
        Symbol_variable** params;
        int num_params;
        int access;
        Symbol_method* overload; // usado para criar uma lista de metodos sobrecarregados
};

typedef struct struct_node* entry;
struct struct_node {
    entry  next;
    Symbol * info;
};

class Symbols_table{
    public:
        Symbols_table();
        ~Symbols_table();
        void destroy_symbols(); // deve ser chamado antes do destrutor;
        char* get_lexeme(Symbol* symbol);
        int insert(char * lexeme, Symbol* symbol);
        Symbol* search(char * lexeme);
        void print_symbol(Symbol* symbol);
        void SET_array_lexemes(char* lexeme);
        virtual void show()=0;  // método que força a tabela de símbolos a ser abstrata
        Symbol** getAtributos();
    protected:
         entry * table ; // array que representa a tabela em si
         int current_position_array_lexemes; // posicao atual do arranjao de lexemas
         char * array_lexemes;  //Arranjão de lexemas da tabela
         int length_array_lexemes; // tamanho atual do arranjao de lexemas
    private:
        int hash(char * lexeme);
};

class Symbols_table_rw: public Symbols_table{
    public:
        Symbols_table_rw();
        void show();
};

class Symbols_table_id: public Symbols_table{ public: void show(); };

class Symbols_table_literal: public Symbols_table{ public: void show(); };
#endif // SYMBOLS_TABLES_H
