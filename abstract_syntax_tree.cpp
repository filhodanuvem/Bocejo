/**
* abstract_syntax_tree.cpp
* @author Claudson Oliveira
* @date   27 de abril de 2011
*
* Arquivo que implementa os nós da ASA
*
**/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "abstract_syntax_tree.h"
#include "visitors.h"

node_expr::~node_expr(){}

node_primitive::node_primitive(int token,int line){
    attr_token = token;
    attr_value = "";
    attr_line = line;
}

node_primitive::node_primitive(int token, char* value,int line){
    attr_value = (char*)malloc(sizeof(char)*strlen(value));
    strcpy(attr_value,value);
    attr_token = token;
    attr_line = line;
}

node_primitive::~node_primitive(){
    free(attr_value);
}

node_name::node_name(Symbol* id,int line){
    attr_id = id;
    attr_expr = NULL;
    attr_line = line;
}

node_name::node_name(Symbol* id,node_expr* expr,int line){
    attr_id = id;
    attr_expr = expr;
    attr_line = line;
}

node_name:: ~node_name(){
    delete attr_expr;
}

node_this::node_this(int line){
    attr_line = line;
}
node_this::~node_this(){}

node_vector::node_vector(node_name* name,node_expr* expr,int line){
    attr_name = name;
    attr_expr = expr;
    attr_line = line;
}

node_vector::~node_vector(){
    delete attr_expr;
    delete attr_name;
}

node_new::node_new(Symbol* info_symbol_table,int line){
    attr_expr      = NULL;
    attr_info_type = info_symbol_table;
    attr_primitive = 0;
    attr_line = line;
}

node_new::node_new(int primitive,node_expr* expr,int line){
    attr_expr      = expr;
    attr_info_type = NULL;
    attr_primitive = primitive;
    attr_line = line;
}

node_new::~node_new(){
    delete attr_expr;
}

node_expr_list::node_expr_list(node_expr* expr, node_expr_list* expr_list,int line){
    attr_expr = expr;
    attr_expr_list = expr_list;
    attr_line = line;
}

node_expr_list::~node_expr_list(){
    delete attr_expr;
    delete attr_expr_list;
}

node_call::node_call(Symbol* id, node_expr_list* expr_list,int line){
    attr_id = id;
    attr_expr_list = expr_list;
    attr_line = line;
}

node_call::~node_call(){
    delete attr_id;
    delete attr_expr_list;
}

node_negation::node_negation(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}

node_negation::~node_negation(){
    delete attr_expr;
}

node_signal::node_signal(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}


node_signal::~node_signal(){
    delete attr_expr;
}

node_length::node_length(int line){
    attr_line = line;
}

node_length::~node_length(){
}

node_op_addition::node_op_addition(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_addition::~node_op_addition(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_boolean::node_op_boolean(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_boolean::~node_op_boolean(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_relational::node_op_relational(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_relational::~node_op_relational(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_bitwise::node_op_bitwise(int operat, node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_bitwise::~node_op_bitwise(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_multiplication::node_op_multiplication(int operat, node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_multiplication::~node_op_multiplication(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_stmt::~node_stmt(){}

node_stmt_list::node_stmt_list(node_stmt* stmt,node_stmt_list* stmt_list,int line){
    attr_stmt      = stmt;
    attr_stmt_list = stmt_list;
    attr_line = line;
}

node_stmt_list::~node_stmt_list(){
    delete attr_stmt;
    delete attr_stmt_list;
}

int node_assign::INIT_ARRAY  = 1;
int node_assign::INIT_OBJECT = 2;
node_assign::node_assign(node_expr* expr_left, node_expr* expr_right,int line){
    attr_expr_left = expr_left;
    attr_expr_right = expr_right;
    attr_line = line;
    type_assing = 0; //0 - primario, 1- atribuição de objetos, 2 - instanciação de objetos
}


node_assign::~node_assign(){
    delete attr_expr_left;
    delete attr_expr_right;
}

node_return::node_return(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}

node_return::~node_return(){
    delete attr_expr;
}

node_println::node_println(node_expr_list* expr_list,int line){
    attr_expr_list = expr_list;
    attr_line = line;
}

node_println::~node_println(){
    delete attr_expr_list;
}

node_read::node_read(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}

node_read::~node_read(){
    delete attr_expr;
}

node_if::node_if(node_expr* expr,node_stmt* stmt,node_stmt* else_,int line){
    attr_expr = expr;
    attr_stmt = stmt;
    attr_stmt_else = else_;
    attr_line = line;
}

node_if::~node_if(){
    delete attr_expr;
    delete attr_stmt;
    delete attr_stmt_else;
}

node_while::node_while(node_expr* expr,node_stmt* stmt,int line){
    attr_expr = expr;
    attr_stmt = stmt;
    attr_line = line;
}

node_while::~node_while(){
    delete attr_expr;
    delete attr_stmt;
}


node_type::node_type(Symbol* id,int line){
    attr_id = id;
    attr_primitive = 0; // não é um tipo primitivo
    attr_array = false; // não é um array
    attr_line = line;
}

node_type::node_type(int primitive, int line){
    attr_primitive = primitive;
    attr_id = NULL; // não é um objeto
    attr_array = false; // não é um array
    attr_line = line;
}

node_type::node_type(int primitive,int array,int line){
    attr_primitive = primitive;
    attr_id = NULL; // não é um objeto
    attr_array = array;
    attr_line = line;
}

node_type::~node_type(){}

node_name_decl::node_name_decl(node_type* type, Symbol* id, node_assign* assign,int line){
    attr_type   = type;
    attr_id     = id;
    attr_assign = assign; //@TODO se for null criar um objeto assign, testar o tipo e atribuir um default
    attr_line = line;
}

node_name_decl::node_name_decl(node_type* type, Symbol* id,int line){
    attr_type   = type;
    attr_id     = id;
    attr_assign = NULL;
    attr_line = line;
}

node_name_decl::~node_name_decl(){
    delete attr_type;
    delete attr_assign;
}

node_var_list::node_var_list(node_name_decl* name,node_var_list* var_list,int line){
    attr_name     = name;
    attr_var_list = var_list;
    attr_line = line;
}

node_var_list::~node_var_list(){
    delete attr_name;
    delete attr_var_list;
}

void node_var_list::insert_list(node_var_list* node){
    node_var_list* p = this->attr_var_list;
    if(p == NULL) this->attr_var_list = node;
    else{
        while(p->attr_var_list != NULL) p = p->attr_var_list;
        p->attr_var_list = node;
    }
}

node_method_list::node_method_list(node_type* type,Symbol* id,node_var_list* param_list,node_stmt_list* stmt_list,node_method_list* method_list,int access,int line){
    attr_type        = type;
    attr_id          = id;
    attr_param_list  = param_list;
  //  attr_scope   = scope;
    attr_stmt_list = stmt_list;
    attr_method_list = method_list;
    attr_access = access;
    attr_line = line;
}

node_method_list::~node_method_list(){
    delete attr_type;
    delete attr_param_list;
    delete attr_stmt_list;
    delete attr_method_list;
}

void node_method_list::insert_list(node_method_list* node){
    node_method_list* p = this->attr_method_list;
    if(p == NULL) this->attr_method_list = node;
    else{
        while(p->attr_method_list != NULL) p = p->attr_method_list;
        p->attr_method_list = node;
    }
}

node_class_list::node_class_list(Symbol* id,node_var_list* var_list,node_method_list* method_list,Symbol* id_father,node_class_list* class_list,int line){
    attr_id          = id;
    attr_var_list    = var_list;
    attr_method_list = method_list;
    attr_id_father   = id_father;
    attr_class_list  = class_list;
    attr_line = line;
}

node_class_list::~node_class_list(){
    delete attr_var_list;
    delete attr_method_list;
    delete attr_id_father;
    delete attr_class_list;
}

node_program::node_program(node_class_list* class_list){
    attr_class_list = class_list;
}

node_program::~node_program(){
    delete attr_class_list;
    delete attr_class_list;
}

// Métodos do padrão Visitor

void node_assign::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_primitive::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

//void node_boolean::accept(Visitor* visitor){
//    if(visitor != NULL ) visitor->visit(this);
//}

void node_call::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_class_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_expr_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_if::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_length::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_method_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_this::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_name::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_vector::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_name_decl::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_negation::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_new::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_addition::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_bitwise::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_boolean::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_relational::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_multiplication::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_println::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_program::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_read::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_return::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_signal::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_stmt_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_type::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}


void node_var_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_while::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

//void node_scope::accept(Visitor* visitor){
//    if(visitor != NULL ) visitor->visit(this);
//}





