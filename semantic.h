#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include "symbols_tables.h"

#define SCOPE_CLASS 0
#define SCOPE_METHOD 1
#define ITS_CALL 1000
#define ITS_ATTR 1001
#define ITS_VECT 1002
#define ITS_THIS 1003
#define ITS_RETURN 1004

class node_name_decl;
class node_method_list;


class scope{
   public:
       scope();
       ~scope();
       void put(Symbol_variable* symbol, char* key);
       void put(Symbol_method* symbol, char* key);
       Symbol_variable* get_variable(char* key);
       Symbol_method* get_method(char* key);
       Symbols_table_id* table_variable;
       Symbols_table_id* table_method;
       Symbol_class* current_class;
       scope* next;
};


void semantic_constructor();
void semantic_destructor();
void begin_class_scope();
void begin_scope();
void end_scope();
void put(Symbol_class* symbol, char* key);
scope* GET_current_scope();
scope* GET_scope_this();
int GET_scope_level();
Symbols_table_id* GET_table_class();
Symbol_class* GET_class(char* key);
bool is_declared(char* vriable,scope* start);
bool is_declared_inherited(char* variable,scope* start);
Symbol_variable* create_variable(node_name_decl* node);
Symbol_method* create_method(node_method_list* node);
bool is_new_signature(Symbol_method* meth,Symbol_method* saved);
#endif // SEMANTIC_H_INCLUDED
