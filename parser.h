#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#define PRINT_FLUX 0 //flag que mostra ou não o fluxo do parser

class node_assign;
class node_boolean;
class node_stmt;
class node_call;
class node_class_list;
class node_expr_list;
class node_if;
class node_length;
class node_literal;
class node_method_list;
class node_this;
class node_name;
class node_vector;
class node_name_decl;
class node_negation;
class node_new;
class node_number;
class node_op_bitwise;
class node_op_addition;
class node_op_boolean;
class node_op_multiplication;
class node_op_relational;
class node_primitive;
class node_println;
class node_program;
class node_read;
class node_return;
class node_signal;
class node_stmt_list;
class node_type;
class node_var_decl;
class node_var_list;
class node_while;
class node_expr;

void parser_constructor();
void parser_destructor();
void eat(int token);
void eat_or_skip(int token, int* synch );
void skip(int* synch);

//NÃO TERMINAIS
node_program* g_program();
node_class_list* g_program_();
node_class_list* g_class_decl();
Symbol* g_extends();
void g_local_decl(node_var_list** var_list,node_method_list** method_list);
node_var_list* g_var_decl(node_var_list** var_list);
node_var_list* g_id_list(node_type** type, node_var_list** var_list);
node_var_list* g_id_list_(node_type** type, node_var_list** var_list);
node_assign* g_assign(node_expr** expr);
node_method_list* g_method_decl();
node_type* g_static_or_type();
node_var_list* g_formal_list(node_var_list** param_list);
node_var_list* g_formal_list_(node_var_list** param_list);
node_type* g_type_or_id();
node_type* g_type();
bool g_bracket();
node_stmt_list* g_stmt_list();
node_stmt* g_stmt();
node_stmt* g_id_compl(Symbol** id,node_var_list** var_list);
node_stmt* g_id_compl_dot(node_name** name);
node_stmt* g_stmt_out_or_in();
node_stmt* g_else_stmt();
node_expr_list* g_expr_list();
node_expr_list* g_expr_list_();
node_expr* g_expr();
node_new* g_expr_new();
node_expr* g_expr_();
node_expr* g_binop2();
node_expr* g_binop2_();
node_expr* g_binop3();
node_expr* g_binop3_();
node_expr* g_binop4();
node_expr* g_binop4_();
node_expr* g_binop5();
node_expr* g_binop5_();
node_expr* g_binop6();
node_expr* g_binop6_();
node_expr* g_binop7();
node_expr* g_binop7_();
node_expr* g_unop();
node_expr* g_primary();
node_expr* g_struct(node_expr** arg);
node_expr* g_dot();
void g_parent();
void g_dot_();

#endif // PARSER_H_INCLUDED
