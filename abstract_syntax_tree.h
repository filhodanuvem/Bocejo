#include "symbols_tables.h"

class Visitor;

class node_stmt{
    public:
        virtual ~node_stmt();
        virtual void accept(Visitor* visitor)=0;
        int attr_line;
};

class node_expr: public node_stmt {
    public:
        virtual ~node_expr();
        virtual void accept(Visitor* visitor)=0;
        int attr_line;
       // node_expr* attr_expr;
};

class node_primitive: public node_expr{
    public:
        node_primitive(int token,int line);
        node_primitive(int token, char* value,int line);
        ~node_primitive();
        char* attr_value;
        int attr_token;
        void accept(Visitor* visitor);
};

class node_this: public node_expr{
    public:
        node_this(int line);
        ~node_this();
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_new: public node_expr{
    public:
        ~node_new();
        node_new(Symbol* info_symbol_table,int line);
        node_new(int primitive,node_expr* expr,int line);
        Symbol* attr_info_type;
        node_expr* attr_expr;
        int attr_primitive;
        void accept(Visitor* visitor);
};


class node_expr_list{
    public:
        node_expr_list(node_expr* expr, node_expr_list* expr_list,int line);
        ~node_expr_list();
        node_expr* attr_expr;
        node_expr_list* attr_expr_list;
        void accept(Visitor* visitor);
        int attr_line;
};

class node_negation: public node_expr{
    public:
        ~node_negation();
        node_negation(node_expr* expr,int line);
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_signal: public node_expr{
    public:
        ~node_signal();
        node_signal(node_expr* expr,int line);
        node_expr* attr_expr ;
        void accept(Visitor* visitor);
};


class node_op_addition: public node_expr{
    public:
        ~node_op_addition();
        node_op_addition(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_multiplication: public node_expr{
    public:
        ~node_op_multiplication();
        node_op_multiplication(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_relational: public node_expr{
    public:
        ~node_op_relational();
        node_op_relational(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_boolean: public node_expr{
    public:
        ~node_op_boolean();
        node_op_boolean(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_bitwise: public node_expr{
    public:
        ~node_op_bitwise();
        node_op_bitwise(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_name: public node_expr{
    public:
        ~node_name();
        node_name(Symbol* id,int line); // para variavel simples
        node_name(Symbol* id, node_expr* expr,int line); // atributo
        Symbol* attr_id; // entrada pra tabela de ID
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_vector: public node_expr {
    public:
        node_vector(node_name* name,node_expr* expr,int line);
        ~node_vector();
        node_name* attr_name;
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_length: public node_expr{
    public:
        ~node_length();
        node_length(int line);
        void accept(Visitor* visitor);
};

class node_call: public node_expr{
    public:
        ~node_call();
        node_call(Symbol* id, node_expr_list* expr_list,int line);
        Symbol* attr_id;
        node_expr* attr_expr;
        node_expr_list* attr_expr_list;
        void accept(Visitor* visitor);
};

class node_stmt_list: public node_stmt{
    public:
        ~node_stmt_list();
        node_stmt_list(node_stmt* stmt,node_stmt_list* stmt_list,int line);
        node_stmt* attr_stmt;
        node_stmt_list* attr_stmt_list;
        void accept(Visitor* visitor);
};

class node_assign: public node_stmt{
    public:
        ~node_assign();
        node_assign(node_expr* expr_left, node_expr* expr_right,int line);
        node_expr* attr_expr_left;
        node_expr* attr_expr_right;
        int type_assing;
        static int INIT_ARRAY;
        static int INIT_OBJECT;
        void accept(Visitor* visitor);
};

class node_return: public node_stmt{
    public:
        ~node_return();
        node_return(node_expr* expr,int line);
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_println: public node_stmt{
    public:
        ~node_println();
        node_println(node_expr_list* expr_list,int line);
        node_expr_list* attr_expr_list;
        void accept(Visitor* visitor);
};

class node_read: public node_stmt{
    public:
        ~node_read();
        node_read(node_expr* expr,int line);
        node_expr* attr_expr;
        void accept(Visitor* visitor);
};

class node_if: public node_stmt{
    public:
        ~node_if();
        node_if(node_expr* expr,node_stmt* stmt,node_stmt* stmt_else,int line);
        node_expr* attr_expr;
        node_stmt* attr_stmt;
        node_stmt* attr_stmt_else;
        void accept(Visitor* visitor);
};

class node_while: public node_stmt{
    public:
        node_while(node_expr* expr,node_stmt* stmt,int line);
        ~node_while();
        node_expr* attr_expr;
        node_stmt* attr_stmt;
        void accept(Visitor* visitor);
};


class node_type{
    public:
        ~node_type();
        node_type(Symbol* id,int line);
        node_type(int primitive,int line);
        node_type(int primitive,int array,int line);
        Symbol* attr_id; // Tem prioridade sobre o int primitivo, se este não for null, sera o tipo
        int attr_primitive;
        bool attr_array;
        int attr_line;
        void accept(Visitor* visitor);
};


class node_name_decl{
    public:
        ~node_name_decl();
        node_name_decl(node_type* type,Symbol* id,node_assign* assign, int line);
        node_name_decl(node_type* type,Symbol* id,int line);
        node_type* attr_type;
        Symbol* attr_id;
        node_assign* attr_assign;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_var_list: public node_stmt{
    public:
        ~node_var_list();
        node_var_list(node_name_decl* nome,node_var_list* var_list, int line);
        node_name_decl* attr_name;
        node_var_list* attr_var_list;
        int attr_line;
        void accept(Visitor* visitor);
        void insert_list(node_var_list* node);

};

class node_method_list{
    public:
        ~node_method_list();
        node_method_list(node_type* type,Symbol* id,node_var_list* param_list, node_stmt_list* stmt_list,node_method_list* method_list,int access,int line);
        node_type* attr_type;
        Symbol* attr_id;
        node_var_list* attr_param_list;
        node_stmt_list* attr_stmt_list;
        node_method_list* attr_method_list;
        int attr_line;
        int attr_access;
        void insert_list(node_method_list* node);
        void accept(Visitor* visitor);
};

class node_class_list{
    public:
        ~node_class_list();
        node_class_list(Symbol* id,node_var_list* var_list, node_method_list* method_list,Symbol* id_father,node_class_list* class_list,int line); // duvida sobre variaveis e metodos em ramos diferentes
        Symbol* attr_id;
        node_var_list* attr_var_list;
        node_method_list* attr_method_list;
        Symbol* attr_id_father;
        node_class_list* attr_class_list;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_program{
    public:
        ~node_program();
        node_program(node_class_list* class_list);
        node_class_list* attr_class_list;
        void accept(Visitor* visitor);
};
