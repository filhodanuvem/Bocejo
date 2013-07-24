#ifndef VISITORS_H
#define VISITORS_H

#include "semantic.h"

class node_assign;
class node_boolean;
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
//class node_scope;

class Visitor{
    public:
        virtual void visit(node_assign* node)=0;
        virtual void visit(node_primitive* node)=0;
        virtual void visit(node_call* node)=0;
        virtual void visit(node_class_list* node)=0;
        virtual void visit(node_expr_list* node)=0;
        virtual void visit(node_if* node)=0;
        virtual void visit(node_length* node)=0;
        virtual void visit(node_method_list* node)=0;
        virtual void visit(node_this* node)=0;
        virtual void visit(node_name* node)=0;
        virtual void visit(node_vector* node)=0;
        virtual void visit(node_name_decl* node)=0;
        virtual void visit(node_negation* node)=0;
        virtual void visit(node_new* node)=0;
        virtual void visit(node_op_bitwise* node)=0;
        virtual void visit(node_op_addition* node)=0;
        virtual void visit(node_op_boolean* node)=0;
        virtual void visit(node_op_multiplication* node)=0;
        virtual void visit(node_op_relational* node)=0;
        virtual void visit(node_println* node)=0;
        virtual void visit(node_program* node)=0;
        virtual void visit(node_read* node)=0;
        virtual void visit(node_return* node)=0;
        virtual void visit(node_signal* node)=0;
        virtual void visit(node_stmt_list* node)=0;
        virtual void visit(node_type* node)=0;
        virtual void visit(node_var_list* node)=0;
        virtual void visit(node_while* node)=0;
};


class Visitor_print_AST: public Visitor{
    public:
        Visitor_print_AST();
        void visit(node_assign* node);
        void visit(node_primitive* node);
        void visit(node_call* node);
        void visit(node_class_list* node);
        void visit(node_expr_list* node);
        void visit(node_if* node);
        void visit(node_length* node);
        void visit(node_method_list* node);
        void visit(node_this* node);
        void visit(node_name* node);
        void visit(node_vector* node);
        void visit(node_name_decl* node);
        void visit(node_negation* node);
        void visit(node_new* node);
        void visit(node_op_bitwise* node);
        void visit(node_op_addition* node);
        void visit(node_op_boolean* node);
        void visit(node_op_multiplication* node);
        void visit(node_op_relational* node);
        void visit(node_println* node);
        void visit(node_program* node);
        void visit(node_read* node);
        void visit(node_return* node);
        void visit(node_signal* node);
        void visit(node_stmt_list* node);
        void visit(node_type* node);
        void visit(node_var_list* node);
        void visit(node_while* node);
    private:
        void identation();
        int level;
};

class Visitor_semantic: public Visitor{
    public:
        Visitor_semantic();
        void visit(node_assign* node);
        void visit(node_call* node);
        void visit(node_class_list* node);
        void visit(node_expr_list* node);
        void visit(node_if* node);
        void visit(node_length* node);
        void visit(node_method_list* node);
        void visit(node_this* node);
        void visit(node_name* node);
        void visit(node_vector* node);
        void visit(node_name_decl* node);
        void visit(node_negation* node);
        void visit(node_new* node);
        void visit(node_op_bitwise* node);
        void visit(node_op_addition* node);
        void visit(node_op_boolean* node);
        void visit(node_op_multiplication* node);
        void visit(node_op_relational* node);
        void visit(node_primitive* node);
        void visit(node_println* node);
        void visit(node_program* node);
        void visit(node_read* node);
        void visit(node_return* node);
        void visit(node_signal* node);
        void visit(node_stmt_list* node);
        void visit(node_type* node);
        void visit(node_var_list* node);
        void visit(node_while* node);
        char* GET_type_name(int token);
     private:
        Symbols_table_id* table_id;
        Symbols_table_id* table_class;
        int attribute_inherited_int  ;
        int attribute_sintetic_int  ;
        char* attribute_inherited_char;
        char* attribute_sintetic_char;
        int current_return_int ;
        char* current_return_char ;
        int has_main;
        int is_array;
        int is_call;
        int is_attr;
};
#endif // VISITORS_H
