#include "visitor_ir.h"

class label;
class LabelList;
class temp;


class ir_stmt{
    public:
        virtual ~ir_stmt();
        virtual ir_stmt* accept(Visitor_IR* visitor)=0;
};


class ir_expr: public ir_stmt {
    public:
        virtual ~ir_expr();
        virtual ir_expr* accept(Visitor_IR* visitor)=0;
};


class ir_stmt_list: public ir_stmt {
    public:
        ir_stmt_list(ir_stmt* stmt, ir_stmt_list* stmt_list);
        ~ir_stmt_list();
        ir_stmt* attr_stmt;
        ir_stmt_list* attr_stmt_list;
        ir_stmt_list* accept(Visitor_IR* visitor);
};

class ir_expr_list:public ir_expr {
    public:
        ir_expr_list(ir_expr* e,ir_expr_list* el);
        ~ir_expr_list();
        ir_expr* attr_expr;
        ir_expr_list* attr_expr_list;
        ir_expr* accept(Visitor_IR* visitor);
};

class ir_const: public ir_expr{
    public:
        ir_const (int i);
        ~ir_const();
        int attr_i;
        ir_expr* accept(Visitor_IR* visitor);
};

class ir_constf: public ir_expr{
    public:
        ir_constf (float f);
        ~ir_constf();
        float attr_f;
        ir_expr* accept(Visitor_IR* visitor);
};


class ir_name: public ir_expr{
    public:
        ir_name(label* n);
        ~ir_name();
        label* attr_name;
        ir_expr* accept(Visitor_IR* visitor);
};

class ir_temp: public ir_expr{
    public:
        ir_temp(temp* t);
        ~ir_temp();
        temp* attr_temp;
        ir_expr* accept(Visitor_IR* visitor);
};


class ir_binop: public ir_expr{
    public:
        ir_binop (int op, ir_expr* e1, ir_expr* e2);
        ~ir_binop();
        int attr_op;
        ir_expr* attr_exp1;
        ir_expr* attr_exp2;
        ir_expr* accept(Visitor_IR* visitor);
};


class ir_mem: public ir_expr{
    public:
        ir_mem (ir_expr* e);
        ~ir_mem();
        ir_expr* attr_expr;
        ir_expr* accept(Visitor_IR* visitor);
};



class ir_call: public ir_expr{
    public:
        ir_call(ir_name* n, ir_expr_list* el);
        ~ir_call();
        ir_name* attr_name;
        ir_expr_list* attr_el;
        ir_stmt_list* attr_params;
        ir_expr* accept(Visitor_IR* visitor);
};


class ir_eseq: public ir_expr{
    public:
        ir_eseq(ir_stmt* s, ir_expr* e);
        ~ir_eseq();
        ir_stmt* attr_stmt;
        ir_expr* attr_expr;
        ir_expr* accept(Visitor_IR* visitor);
};


//extends STMT

class ir_move: public ir_stmt{
    public:
        ir_move(ir_expr* e1, ir_expr* e2);
        ~ir_move();
        ir_expr* attr_expr1;
        ir_expr* attr_expr2;
        ir_stmt* accept(Visitor_IR* visitor);

};

class ir_expr_void: public ir_stmt{
    public:
        ir_expr_void(ir_expr* e);
        ~ir_expr_void();
        ir_expr* attr_expr;
        ir_stmt* accept(Visitor_IR* visitor);
};

class ir_jump: public ir_stmt{
    public:
        ir_jump(ir_expr* e,LabelList* labels);
        ~ir_jump();
        ir_expr* attr_expr;
        LabelList* attr_labels;
        ir_stmt* accept(Visitor_IR* visitor);
};

class ir_cjump: public ir_stmt{
    public:
        ir_cjump(int op, ir_expr* e1, ir_expr* e2, label* t, label* f);
        ~ir_cjump();
        int attr_op;
        ir_expr* attr_expr1;
        ir_expr* attr_expr2;
        label* attr_true;
        label* attr_false;
        ir_stmt* accept(Visitor_IR* visitor);

};

class ir_seq: public ir_stmt{
    public:
        ir_seq(ir_stmt* s1, ir_stmt* s2);
        ~ir_seq();
        ir_stmt* attr_stmt1;
        ir_stmt* attr_stmt2;
        ir_stmt* accept(Visitor_IR* visitor);
};

class ir_label: public ir_stmt{
    public:
        ir_label(label* n);
        ~ir_label();
        label* attr_n;
        ir_stmt* accept(Visitor_IR* visitor);
};
// classe especial para transformar um expr_list num stmt_list canonizado
class ir_params: public ir_expr {
    public:
        ir_params(ir_stmt_list* stmt_list);
        ~ir_params();
        ir_stmt_list* attr_sl;
        ir_expr* accept(Visitor_IR* visitor);
};
